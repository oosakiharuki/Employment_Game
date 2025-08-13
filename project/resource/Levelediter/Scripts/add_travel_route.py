import bpy
import mathutils

class MYADDON_OT_add_travel_route(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_travel_route"
    bl_label = "移動ルートを追加"
    bl_description = "['travel_route']カスタムプロパティを追加します"
    bl_options = {"REGISTER","UNDO"}

    def execute(self,context):
        context.object["start"] = mathutils.Vector((-2,0,0))
        context.object["end"] = mathutils.Vector((2,0,0))

        return {"FINISHED"}