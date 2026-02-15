import bpy
import mathutils

class MYADDON_OT_add_eventTrigger(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_enevt_trigger"
    bl_label = "イベントトリガー作成"
    bl_description = "イベントトリガーを作成します"
    bl_options = {"REGISTER","UNDO"}

    def execute(self,context):
        # 中心点用のICO球
        bpy.ops.mesh.primitive_ico_sphere_add()
        
        context.object["event_trigger"] = "event_trigger"
        context.object["trigger_center"] = mathutils.Vector((0,0,0))
        context.object["trigger_size"] = mathutils.Vector((2,2,2))
        context.object["set_camera"] = ""
        context.object["set_csv"] = ""
 
        return {"FINISHED"}
    
class OBJECT_PT_move_eventTrigger(bpy.types.Panel):
    bl_idname = "obejct_PT_move_event_trigger"
    bl_label = "eventTrigger"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self,context):
        if "event_trigger" in context.object:

            self.layout.prop(context.object,'["trigger_center"]',text="Center")
            self.layout.prop(context.object,'["trigger_size"]',text="Size")
            self.layout.prop(context.object,'["set_camera"]',text="camera")
            self.layout.prop(context.object,'["set_csv"]',text="csv")

