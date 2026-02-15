import bpy

#オペレータ シーン出力
from  .export_scene import MYADDON_OT_export_scene

from .spawn import MYADDON_OT_create_player_spawn

from .spawn import MYADDON_OT_create_enemy_Soldier_spawn
from .spawn import MYADDON_OT_create_enemy_Turret_spawn
from .spawn import MYADDON_OT_create_enemy_Bomb_spawn

from .spawn import MYADDON_OT_create_check_point
from .spawn import MYADDON_OT_create_warp_gate
from .spawn import MYADDON_OT_create_goal
from .spawn import MYADDON_OT_create_move_ground

from .spawn import MYADDON_OT_create_boss_spawn

from .add_event_trigger import MYADDON_OT_add_eventTrigger


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
        self.layout.operator(MYADDON_OT_export_scene.bl_idname,text=MYADDON_OT_export_scene.bl_label)
        

        #プレイヤーを作成
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_player_spawn.bl_idname,text=MYADDON_OT_create_player_spawn.bl_label)
        
        #敵を作成
        self.layout.separator() 
        self.layout.menu(MYADDON_MT_sub_EnemySpawn.bl_idname)

        #ギミックを作成
        self.layout.separator()
        self.layout.menu(MYADDON_MT_sub_StageObjectSpawn.bl_idname)

        self.layout.separator()
        self.layout.operator(MYADDON_OT_add_eventTrigger.bl_idname,text=MYADDON_OT_add_eventTrigger.bl_label)

        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_boss_spawn.bl_idname,text=MYADDON_OT_create_boss_spawn.bl_label)



    # 存じのメニューにサブメニュー追加
    def submenu(self,context):
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)


#敵の作成をまとめた
class MYADDON_MT_sub_EnemySpawn(bpy.types.Menu):
    bl_idname = "myaddon.myaddon_mt_sub_enemy_spawn"
    bl_label = "EnemySpawnを作成"
    bl_description = "EnemySpownを作成します"

    #サブメニュー描画
    def draw(self,context):
        #区切り線
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_enemy_Soldier_spawn.bl_idname,text=MYADDON_OT_create_enemy_Soldier_spawn.bl_label)
        
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_enemy_Turret_spawn.bl_idname,text=MYADDON_OT_create_enemy_Turret_spawn.bl_label)
        
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_enemy_Bomb_spawn.bl_idname,text=MYADDON_OT_create_enemy_Bomb_spawn.bl_label)
        

#ギミック制作をまとめた
class MYADDON_MT_sub_StageObjectSpawn(bpy.types.Menu):
    bl_idname = "myaddon.myaddon_mt_sub_stage_bject_spawn"
    bl_label = "StageObjectSpawnを作成"
    bl_description = "ゲーム内のギミックを作成します"

    #サブメニュー描画
    def draw(self,context):
        #チェックポイント
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_check_point.bl_idname,text=MYADDON_OT_create_check_point.bl_label)
        #ワープゲート
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_warp_gate.bl_idname,text=MYADDON_OT_create_warp_gate.bl_label)
        #ゴール
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_goal.bl_idname,text=MYADDON_OT_create_goal.bl_label)
        #動く足場
        self.layout.separator()
        self.layout.operator(MYADDON_OT_create_move_ground.bl_idname,text=MYADDON_OT_create_move_ground.bl_label)
    
