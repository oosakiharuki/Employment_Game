import bpy

#オペレータ 頂点を伸ばす
from .stretch_vertex import MYADDON_OT_stretch_vertex

#オペレータ ICO球生成
from  .create_ico_sphere import MYADDON_OT_create_ico_sphere

#オペレータ シーン出力
from  .export_scene import MYADDON_OT_export_scene

#from .spawn import MYADDON_OT_spawn_import_symbol

#from .spawn import MYADDON_OT_make_spawn_point

#from .spawn import SpawnNames

from .spawn import MYADDON_OT_create_player_spawn

from .spawn import MYADDON_OT_create_enemy_spawn


# ブレンダーに登録するアドオン情報
bl_info = {
    "name": "05レベルエディタ",
    "author": "Haruki Osaki",
    "version": (1,0),
    "blender": (4,4,1),
    "description": "レベルエディタ",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"
}



#メニュー作成
class TOPBAR_MT_my_menu(bpy.types.Menu):
    bl_idname = "TOPBAR_MT_my_menu"
    bl_label = "MyMenu"
    bl_description = "拡張メニュー by" + bl_info["author"]
        
    #self 呼び出し元のクラスインスタンス
    #context カーソルを合わせたときのポップアップ

    #サブメニュー描画
    def draw(self,context):

        self.layout.operator("wm.url_open_preset",text="Manual",icon='HELP')

        #区切り線
        self.layout.separator()
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname,text=MYADDON_OT_stretch_vertex.bl_label)

        #区切り線
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname,text=MYADDON_OT_create_ico_sphere.bl_label)
             
        #区切り線
        self.layout.separator()
        self.layout.operator(MYADDON_OT_export_scene.bl_idname,text=MYADDON_OT_export_scene.bl_label)

        #区切り線
        #self.layout.separator()
        #self.layout.operator(MYADDON_OT_spawn_import_symbol.bl_idname,text=MYADDON_OT_spawn_import_symbol.bl_label)
        
        #区切り線
        #self.layout.separator()
        #self.layout.operator(MYADDON_OT_make_spawn_point.bl_idname,text=MYADDON_OT_make_spawn_point.bl_label)
               
        #区切り線
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_player_spawn.bl_idname,text=MYADDON_OT_create_player_spawn.bl_label)
        
        #区切り線
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_enemy_spawn.bl_idname,text=MYADDON_OT_create_enemy_spawn.bl_label)


    # 存じのメニューにサブメニュー追加
    def submenu(self,context):
        
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)