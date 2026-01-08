import bpy

from .add_disabled import MYADDON_OT_add_disabled

#パネル
class OBJECT_PT_disabled(bpy.types.Panel):
    bl_idname = "OBJECT_PT_disabled"
    bl_label = "Disabled" # disabled = 無効
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        if "disabled" in context.object:
            self.layout.prop(context.object, '["disabled"]' ,text="disabled") == True
        
        else:
            self.layout.operator(MYADDON_OT_add_disabled.bl_idname)
