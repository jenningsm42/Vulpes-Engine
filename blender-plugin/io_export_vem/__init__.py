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

from . import export_vem


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

    global_scale: FloatProperty(
        name='Scale',
        min=0.01,
        max=1000.0,
        default=1.0,
    )

    check_extension = True

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
        return export_vem.save(context, **keywords)


def menu_func_export(self, context):
    self.layout.operator(ExportVEM.bl_idname, text='Vulpes Engine Mesh (.vem)')


def register():
    bpy.utils.register_class(ExportVEM)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
    bpy.utils.unregister_class(ExportVEM)


if __name__ == '__main__':
    register()
