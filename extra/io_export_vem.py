bl_info = {
    "name": "Export Vulpes Engine Mesh (.vem)",
    "description": "Exports a mesh to the VEM format",
    "author": "Mark Jennings",
    "version": (0, 5),
    "blender": (2, 7, 1),
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

def cmpf(f1, f2):
    return abs(f1 - f2) <= 0.001

def nullTriangle(t): # uvtex[:].data[:].uv
    if cmpf(t[0][0], t[1][0]) and cmpf(t[1][0], t[2][0]) and \
       cmpf(t[0][1], t[1][1]) and cmpf(t[1][1], t[2][1]):
        return True
    return False

def write_data(context, filepath, use_modifiers, use_normals, use_uvcoords, calcTB):    
    sce = bpy.context.scene
    ob = sce.objects.active
    me = ob.to_mesh(sce, use_modifiers, 'PREVIEW')
    me.calc_tangents()
    
    # Set Y-axis to vertical axis
    mat_rot = mathutils.Matrix.Rotation(radians(90.0), 3, 'X')
    
    verts = me.vertices[:]
    faces = me.tessfaces[:]
    uvdata = 0
    if use_uvcoords == True:
        uvdata = me.tessface_uv_textures.active.data[:]
    
    numVerts = 3*len(faces)
    numIndices = 3*len(faces)
    
    header = "VULP"
    version = 5
    flags = 0
    if use_normals == True:
        flags = flags | 1
    if use_uvcoords == True:
        flags = flags | 2
    if calcTB == True:
        flags = flags | 4
    
    f = open(filepath, 'wb+')
    f.write(bytes(header, 'UTF-8'))
    f.write(struct.pack('H', version))
    f.write(struct.pack('B', flags))
    f.write(struct.pack('I', numVerts))
    f.write(struct.pack('I', numIndices))
    
    for face in faces:
        for vert in face.vertices[:]:
            for c in (verts[vert].co*mat_rot)[:]: # VERTEX COORDINATES
                f.write(struct.pack('f', c))
    idx = 0
    for face in faces:
        for i in range(0, 3):
            f.write(struct.pack('I', idx)) # INDICES
            idx += 1
    if use_normals == True:
        for face in faces:
            for vert in face.vertices[:]:
                for n in (verts[vert].normal*mat_rot)[:]: # VERTEX NORMALS
                    f.write(struct.pack('f', n))
    if calcTB == True:
        normals = []
        tangents = []
        bitangents = []
        for face in me.polygons:
            for vert in [me.loops[i] for i in face.loop_indices]:
                tangents.append(vert.tangent)
                normals.append(vert.normal)
                bitangents.append(vert.bitangent_sign * normals[-1].cross(tangents[-1]))
                
        for tangent in tangents[:]:
            for c in tangent[:]:
                f.write(struct.pack('f', c))
        
        for bitangent in bitangents[:]:
            for c in bitangent[:]:
                f.write(struct.pack('f', c))
    if use_uvcoords == True:
        for face, uv in zip(faces, uvdata):
            uvc = 0 # count 0,1,2 through vertices of faces (for UV coords)
            for i in face.vertices:
                f.write(struct.pack('f', uv.uv[uvc][0])) # UV COORDINATES
                f.write(struct.pack('f', 1.0 - uv.uv[uvc][1]))
                uvc = uvc + 1
    
    f.close()
    print("Exported VEM")
    
    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportData(Operator, ExportHelper):
    """Exports a mesh to the VEM format"""
    bl_idname = "export_mesh.vem"  # important since its how bpy.ops.import_test.data is constructed
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

    setting_calc_tb = BoolProperty(
            name="Calculate Tangent and Bitangents",
            description="Calculate tangent and bitangent vectors per vertex",
            default=True,
            )

    def execute(self, context):
        return write_data(context, self.filepath, self.setting_apply_modifiers, self.setting_use_normals, self.setting_use_uvcoords, self.setting_calc_tb)

# For dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportData.bl_idname, text="Vulpes Engine Mesh (.vem)")

def register():
    bpy.utils.register_class(ExportData)
    bpy.types.INFO_MT_file_export.append(menu_func_export)

def unregister():
    bpy.utils.unregister_class(ExportData)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
    bpy.ops.export_mesh.vem('INVOKE_DEFAULT')


