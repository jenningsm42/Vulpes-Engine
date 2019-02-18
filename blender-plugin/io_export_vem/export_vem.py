import operator
import struct

import bmesh
import bpy
from mathutils import Matrix


class NoSelectedObjectException(Exception):
    pass


class ObjectNotMeshException(Exception):
    pass


class NoUVLayersException(Exception):
    pass


class NoArmatureException(Exception):
    pass


class TooManyBonesException(Exception):
    pass


BONES_PER_VERTEX = 4
MAXIMUM_BONES = 255


def triangulate_mesh(mesh):
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(mesh)
    bm.free()


def write_file(filepath,
               object,
               depsgraph,
               apply_modifiers,
               export_normals,
               export_uvcoords,
               export_tb,
               export_armature_weights,
               global_matrix):
    # Obtain triangulated mesh
    try:
        mesh = object.to_mesh(depsgraph, apply_modifiers)
    except RuntimeError:
        raise ObjectNotMeshException()

    triangulate_mesh(mesh)

    # Apply object space transformation
    mesh.transform(global_matrix @ object.matrix_world)

    # Flip normals if negatively scaled
    if object.matrix_world.determinant() < 0.0:
        mesh.flip_normals()

    loops = mesh.loops[:]
    faces = mesh.polygons[:]

    # Vertices may be duplicated to account for per-face data
    vertex_count = len(faces) * 3

    if export_normals:
        mesh.calc_normals_split()

    if export_tb:
        if len(mesh.uv_layers) > 0:
            mesh.calc_tangents(uvmap=mesh.uv_layers.active.name)
        else:
            mesh.calc_tangents()

    uv_coords = None
    if export_uvcoords:
        if not mesh.uv_layers:
            raise NoUVLayersException()

        uv_coords = mesh.uv_layers.active.data[:]

    # Header info
    magic = b'VULP'
    version = 5
    flags = 0
    if export_normals:
        flags |= 2**0
    if export_uvcoords:
        flags |= 2**1
    if export_tb:
        flags |= 2**2
    if export_armature_weights:
        flags |= 2**3

    # Write data
    with open(filepath, 'wb+') as f:
        f.write(magic)
        f.write(struct.pack('H', version))
        f.write(struct.pack('B', flags))
        f.write(struct.pack('I', vertex_count))
        f.write(struct.pack('I', vertex_count))

        # Write vertices
        vertices = [None] * vertex_count * 3

        index = 0
        for face in faces:
            for loop_index in face.loop_indices:
                vertex_index = loops[loop_index].vertex_index
                for coordinate in mesh.vertices[vertex_index].co[:]:
                    vertices[index] = coordinate
                    index += 1

        vertices_buffer = struct.pack(
            '{}f'.format(len(vertices)),
            *vertices)

        f.write(vertices_buffer)

        # Write face indices
        face_indices_buffer = struct.pack(
            '{}I'.format(vertex_count),
            *list(range(vertex_count)))

        f.write(face_indices_buffer)

        # Write normals
        if export_normals:
            normals = [None] * vertex_count * 3

            index = 0
            for face in faces:
                for loop_index in face.loop_indices:
                    for coordinate in loops[loop_index].normal[:]:
                        normals[index] = coordinate
                        index += 1

            normals_buffer = struct.pack(
                '{}f'.format(len(normals)),
                *normals)

            f.write(normals_buffer)

        # Write tangent and bitangent vectors
        if export_tb:
            tangents = [None] * vertex_count * 3
            bitangents = [None] * vertex_count * 3

            index = 0
            for face in faces:
                for loop_index in face.loop_indices:
                    zipped_coordinates = zip(
                        loops[loop_index].tangent[:],
                        loops[loop_index].bitangent[:])
                    for tangent_coord, bitangent_coord in zipped_coordinates:
                        tangents[index] = coordinate
                        bitangents[index] = coordinate
                        index += 1

            tangents_buffer = struct.pack(
                '{}f'.format(len(tangents)),
                *tangents)
            bitangents_buffer = struct.pack(
                '{}f'.format(len(bitangents)),
                *bitangents)

            f.write(tangents_buffer)
            f.write(bitangents_buffer)

        # Write UV coordinates
        if export_uvcoords:
            uvs = [None] * vertex_count * 2

            index = 0
            for face in faces:
                for loop_index in face.loop_indices:
                    uvs[index] = uv_coords[loop_index].uv[0]
                    uvs[index+1] = 1.0 - uv_coords[loop_index].uv[1]
                    index += 2

            uvs_buffer = struct.pack(
                '{}f'.format(len(uvs)),
                *uvs)

            f.write(uvs_buffer)

        # Write vertex weights for parent armature
        if export_armature_weights:
            armature = object.find_armature()
            if armature is None:
                raise NoArmatureException()

            if len(armature.pose.bones) > MAXIMUM_BONES:
                raise TooManyBonesException()

            # Map bone names to indices
            bone_name_to_index = {}
            for index, bone in enumerate(armature.pose.bones):
                bone_name_to_index[bone.name] = index

                # {bone index} - {bone name length} - {bone name}
                f.write(struct.pack('B', index))
                f.write(struct.pack('B', min(len(bone.name), 255)))
                f.write(bone.name[:255].encode('utf8'))

            # Map vertex group index to bone index
            group_to_bone_index = {}
            for index, group in enumerate(object.vertex_groups):
                # There may be non-armature related vertex groups
                bone_index = bone_name_to_index.get(group.name)
                if bone_index:
                    group_to_bone_index[index] = bone_index

            # Write per-vertex bone indices and weights
            bone_indices = [None] * vertex_count * BONES_PER_VERTEX
            vertex_weights = [None] * vertex_count * BONES_PER_VERTEX

            index = 0
            for face in faces:
                for loop_index in face.loop_indices:
                    vertex_index = loops[loop_index].vertex_index
                    groups = mesh.vertices[vertex_index].groups[:]

                    # Ensure that vertex groups are armature-related only
                    groups = [group for group in groups
                              if group.group in group_to_bone_index]

                    # Take the largest weighted vertex groups
                    if len(groups) > BONES_PER_VERTEX:
                        groups.sort(key=operator.attrgetter('weight'),
                                    reverse=True)
                        groups = groups[:BONES_PER_VERTEX]

                    for group in groups:
                        bone_indices[index] = group_to_bone_index[group.group]
                        vertex_weights[index] = group.weight
                        index += 1

                    # Need to have equal number of bones per vertex
                    for _ in range(BONES_PER_VERTEX - len(groups)):
                        bone_indices[index] = 0
                        vertex_weights[index] = 0
                        index += 1

            bone_indices_buffer = struct.pack(
                '{}I'.format(len(bone_indices)),
                *bone_indices)
            vertex_weights_buffer = struct.pack(
                '{}f'.format(len(vertex_weights)),
                *vertex_weights)

            f.write(bone_indices_buffer)
            f.write(vertex_weights_buffer)

    print('Saved VEM file')
    return {'FINISHED'}


def save(context,
         filepath,
         *,
         use_mesh_modifiers,
         use_normals,
         use_uvcoords,
         use_tb,
         use_armature,
         global_matrix):
    if not context.selected_objects:
        raise NoSelectedObjectException()

    object = context.selected_objects[0]
    depsgraph = context.depsgraph

    return write_file(
        filepath,
        object,
        depsgraph,
        use_mesh_modifiers,
        use_normals,
        use_uvcoords,
        use_tb,
        use_armature,
        global_matrix)
