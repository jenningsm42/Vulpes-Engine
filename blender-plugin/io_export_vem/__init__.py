import bpy
from bpy.props import (
    BoolProperty,
    FloatProperty,
    StringProperty,
)
from bpy_extras.io_utils import (
    ExportHelper,
    axis_conversion,
    orientation_helper,
)
from mathutils import Matrix

from . import export_vem, export_ves


bl_info = {
    'name': 'Vulpes Engine Mesh Format',
    'author': 'Mark Jennings',
    'version': (0, 6, 0),
    'blender': (2, 80, 0),
    'location': 'File > Import-Export',
    'description': 'Export mesh for use in Vulpes Engine',
    'category': 'Import-Export',
}


@orientation_helper(axis_forward='-Z', axis_up='Y')
class ExportVEM(bpy.types.Operator, ExportHelper):
    '''Save a VEM file'''

    bl_idname = 'export.vem'
    bl_label = 'Export VEM'
    bl_options = {'PRESET'}

    filename_ext = '.vem'
    filter_glob: StringProperty(
        default='*.vem',
        options={'HIDDEN'},
    )

    use_mesh_modifiers: BoolProperty(
        name='Apply Modifiers',
        description='Apply modifiers',
        default=True,
    )

    use_normals: BoolProperty(
        name='Write Normals',
        description='Export vertex normals',
        default=True,
    )

    use_uvcoords: BoolProperty(
        name='Write UV',
        description='Export vertex UV coordinates',
        default=True,
    )

    use_tb: BoolProperty(
        name='Write Tangents and Bitangents',
        description='Export vertex tangent and bitangent vectors',
        default=True,
    )

    use_armature: BoolProperty(
        name='Write armature weights',
        description='Export vertex weights for the parent armature',
        default=False,
    )

    global_scale: FloatProperty(
        name='Scale',
        min=0.01,
        max=1000.0,
        default=1.0,
    )

    check_extension = True

    @classmethod
    def poll(cls, context):
        object = context.object
        return object and object.type == 'MESH'

    def execute(self, context):
        keywords = self.as_keywords(ignore=('axis_forward',
                                            'axis_up',
                                            'global_scale',
                                            'check_existing',
                                            'filter_glob'))

        global_matrix = (Matrix.Scale(self.global_scale, 4) @
                         axis_conversion(to_forward=self.axis_forward,
                                         to_up=self.axis_up).to_4x4())

        keywords['global_matrix'] = global_matrix

        try:
            return export_vem.save(context, **keywords)
        except export_vem.NoSelectedObjectException:
            self.report({'ERROR'}, 'No object is currently selected')
        except export_vem.ObjectNotMeshException:
            self.report({'ERROR'}, 'Selected object is not a mesh')
        except export_vem.NoUVLayersException:
            self.report({'ERROR'}, 'Selected object has no UV layers')
        except export_vem.NoArmatureException:
            self.report({'ERROR'}, 'No parent armature found')
        except export_vem.TooManyBonesException:
            self.report(
                {'ERROR'},
                'Too many bones, VEM files support up to 255 bones currently')

        return {'FINISHED'}


@orientation_helper(axis_forward='-Z', axis_up='Y')
class ExportVES(bpy.types.Operator, ExportHelper):
    '''Save a VES file'''

    bl_idname = 'export.ves'
    bl_label = 'Export VES'
    bl_options = {'PRESET'}

    filename_ext = '.ves'
    filter_glob: StringProperty(
        default='*.ves',
        options={'HIDDEN'},
    )

    check_extension = True

    @classmethod
    def poll(cls, context):
        object = context.object
        return object and object.type == 'ARMATURE'

    def execute(self, context):
        keywords = self.as_keywords(ignore=('axis_forward',
                                            'axis_up',
                                            'global_scale',
                                            'check_existing',
                                            'filter_glob'))

        global_matrix = axis_conversion(to_forward=self.axis_forward,
                                        to_up=self.axis_up).to_4x4()

        keywords['global_matrix'] = global_matrix

        try:
            return export_ves.save(context, **keywords)
        except export_ves.NoSelectedObjectException:
            self.report({'ERROR'}, 'No object is currently selected')
        except export_ves.ObjectNotArmatureException:
            self.report({'ERROR'}, 'Selected object is not a mesh')
        except export_ves.TooManyBonesException:
            self.report(
                {'ERROR'},
                'Too many bones, VES files support up to 255 bones currently')

        return {'FINISHED'}


classes = (
    ExportVEM,
    ExportVES,
)


def menu_func_export_vem(self, context):
    self.layout.operator(ExportVEM.bl_idname, text='Vulpes Engine Mesh (.vem)')


def menu_func_export_ves(self, context):
    self.layout.operator(ExportVES.bl_idname, text='Vulpes Engine Skeleton (.ves)')


def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export_vem)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export_ves)


def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)

    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export_vem)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export_ves)


if __name__ == '__main__':
    register()
