bl_info = {
	"name": "Export Vulpes Engine Mesh (.vem)",
	"description": "Exports a mesh to the VEM format",
	"author": "Mark Jennings",
	"version": (0, 1),
	"blender": (2, 7, 0),
	"api": 36079,
	"location": "File > Export > Vulpes Engine Mesh (.vem)",
	"warning": "",
	"category": "Import-Export"
}


import bpy
from math import radians
import mathutils
import struct
import itertools


def write_some_data(context, filepath, use_modifiers, use_normals, use_uvcoords):    
    sce = bpy.context.scene
    ob = sce.objects.active
    me = ob.to_mesh(sce, use_modifiers, 'PREVIEW')
    
    poly = me.polygons
    verts = me.vertices[:]
    faces = me.tessfaces[:]
    uvdata = 0
    if use_uvcoords == True:
        uvdata = me.tessface_uv_textures.active.data[:]
    
    numVerts = 3*len(poly)
    numIndices = 3*len(poly)
    
    header = "VULP"
    version = 2
    flags = 0
    if use_normals == True:
        flags = flags | 1
    if use_uvcoords == True:
        flags = flags | 2
	
    f = open(filepath, 'wb+')
    f.write(bytes(header, 'UTF-8'))
    f.write(struct.pack('H', version))
    f.write(struct.pack('B', flags))
    f.write(struct.pack('I', numVerts))
    f.write(struct.pack('I', numIndices))
    
    for face in poly:
        for vert in face.vertices[:]:
            for c in verts[vert].co[:]: # VERTEX COORDINATES
                f.write(struct.pack('f', c))
    #for face in poly:
    #    for vert in face.vertices[:]:
    #        f.write(struct.pack('H', vert))
    idx = 0
    for face in poly:
        for i in range(0, 3):
            f.write(struct.pack('H', idx))
            idx += 1
    if use_normals == True:
        for face in poly:
            for vert in face.vertices[:]:
                for n in verts[vert].normal[:]: # VERTEX NORMALS
                    f.write(struct.pack('f', n))
    if use_uvcoords == True:
        for face, uv in zip(faces, uvdata):
            uvc = 0 # count 0,1,2 through vertices of faces (for UV coords)
            for i in face.vertices:
                f.write(struct.pack('f', uv.uv[uvc][0])) # UV COORDINATES
                f.write(struct.pack('f', uv.uv[uvc][1]))
                uvc = uvc + 1
            
    f.close()
    
    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """Exports a mesh to the VEM format"""
    bl_idname = "export_mesh.vem"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export VEM"

    # ExportHelper mixin class uses this
    filename_ext = ".vem"

    filter_glob = StringProperty(
            default="*.vem",
            options={'HIDDEN'},
            )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    setting_apply_modifiers = BoolProperty(
            name="Apply modifiers",
            description="Applies all modifiers before exporting",
            default=False,
            )

    setting_use_normals = BoolProperty(
            name="Use Normals",
            description="Exports vertex normals",
            default=True,
            )
            
    setting_use_uvcoords = BoolProperty(
            name="Use UV Coordinates",
            description="Exports UV coordinates",
            default=True,
            )

    def execute(self, context):
        return write_some_data(context, self.filepath, self.setting_apply_modifiers, self.setting_use_normals, self.setting_use_uvcoords)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Vulpes Engine Mesh (.vem)")


def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export_mesh.vem('INVOKE_DEFAULT')
