import struct

import bpy


class NoSelectedObjectException(Exception):
    pass


class ObjectNotArmatureException(Exception):
    pass


class TooManyBonesException(Exception):
    pass


MAXIMUM_BONES = 255


def get_bone_rotation(bone):
    ''' Converts bone rotation to a quaternion '''
    if bone.rotation_mode == 'QUATERNION':
        return bone.quaternion
    elif bone.rotation_mode in ['XYZ', 'XZY', 'YXZ', 'YZX', 'ZXY', 'ZYX']:
        return bone.rotation_euler.to_quaternion()
    else:
        aa = bone.rotation_axis_angle
        return Quaternion((aa[0], aa[1], aa[2]), aa[3])


def write_file(filepath, object, scene):
    if object.type != 'ARMATURE':
        raise ObjectNotArmatureException()

    if len(object.pose.bones) > MAXIMUM_BONES:
        raise TooManyBonesException()

    original_action = object.animation_data.action
    original_frame = scene.frame_current

    with open(filepath, 'wb+') as f:
        magic = b'VULS'

        f.write(magic)
        f.write(struct.pack('B', len(object.pose.bones)))
        f.write(struct.pack('B', min(len(bpy.data.actions), 255)))
        f.write(struct.pack('f', scene.render.fps))

        # Map bone names to indices
        for index, bone in enumerate(object.pose.bones):
            # {bone index} - {bone name length} - {bone name}
            f.write(struct.pack('B', index))
            f.write(struct.pack('B', min(len(bone.name), 255)))
            f.write(bone.name[:255].encode('utf8'))

        # Write each action
        for action in bpy.data.actions:
            # Action name
            f.write(struct.pack('B', min(len(action.name), 255)))
            f.write(action.name[:255].encode('utf8'))

            # Number of frames
            frame_count = int(action.frame_range.length)
            f.write(struct.pack('I', frame_count))

            object.animation_data.action = action

            bone_locations = [[None] * frame_count * 3
                              for _ in object.pose.bones]
            bone_quaternions = [[None] * frame_count * 4
                                for _ in object.pose.bones]

            # Simulate each frame and record bone data
            start = int(action.frame_range.x)
            end = int(action.frame_range.y)
            for i in range(start, end):
                scene.frame_set(i)

                for bone_index, bone in enumerate(object.pose.bones):
                    for j, coordinate in enumerate(bone.location):
                        bone_locations[bone_index][
                            3 * (i - start) + j] = coordinate

                    rotation = get_bone_rotation(bone)
                    for j, coordinate in enumerate(rotation):
                        bone_quaternions[bone_index][
                            4 * (i - start) + j] = coordinate

            for locations in bone_locations:
                locations_buffer = struct.pack(
                    '{}f'.format(len(locations)),
                    *locations)
                f.write(locations_buffer)

            for quaternions in bone_quaternions:
                quaternions_buffer = struct.pack(
                    '{}f'.format(len(quaternions)),
                    *quaternions)
                f.write(quaternions_buffer)

    object.animation_data.action = original_action
    scene.frame_set(original_frame)

    print('Saved VES file')
    return {'FINISHED'}


def save(context, filepath, *, global_matrix):
    if not context.selected_objects:
        raise NoSelectedObjectException()

    object = context.selected_objects[0]
    scene = context.scene

    return write_file(filepath, object, scene)
