import bpy

#オペレータ 
class MYADDON_OT_add_disabled(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_disabled"
    bl_label = "Disabled 追加"
    bl_description = "['disabled']カスタムプロパティを追加します"
    bl_options = {"REGISTER","UNDO"}

    def execute(self, context):
        context.object["disabled"] = True

        return {"FINISHED"}
