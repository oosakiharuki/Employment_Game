import bpy

#オペレータ ICO球生成
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_object"
    bl_label = "ICO弾生成"
    bl_description = "ICO弾を生成します"
    #リドゥ アンドゥ可能オプション
    bl_options = {'REGISTER','UNDO'}

    def execute(self,context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO弾を生成しました")
        
        return {'FINISHED'}