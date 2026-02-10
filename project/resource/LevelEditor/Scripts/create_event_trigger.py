import bpy

from .add_event_trigger import MYADDON_OT_add_eventTrigger

class OBJECT_PT_eventTrigger(bpy.types.Panel):
    bl_idname= "OBJECT_PT_eventTrigger"
    bl_label = "eventTrigger"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self,context):
        if "event_trigger" in context.object:
            self.layout.prop(context.object,'["event_trigger"]',text="event_trigger")
            self.layout.prop(context.object,'["trigger_center"]',text="Center")
            self.layout.prop(context.object,'["trigger_size"]',text="Size")
            self.layout.prop(context.object,'["set_camera"]',text="camera")
        
        else:
            self.layout.operator(MYADDON_OT_add_eventTrigger.bl_idname)
