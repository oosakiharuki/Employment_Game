import bpy

from .add_travel_route import MYADDON_OT_add_travel_route

class OBJECT_PT_travel_route(bpy.types.Panel):
    bl_idname = "OBJECT_PT_travel_route"
    bl_label = "travel_route"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self,context):
        if "collider" in context.object:
            #すでにあるプロパティはプロパティを表示
            self.layout.prop(context.object,'["start"]',text="start")
            self.layout.prop(context.object,'["end"]',text="end")

        else:
            # プロパティがなければ、プロパティ追加ボタンを表示
            self.layout.operator(MYADDON_OT_add_travel_route.bl_idname)