import bpy
import math
import bpy_extras

import gpu
import gpu_extras.batch
import copy

import mathutils

import json

import os
import bpy.ops

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


#オペレータ シーン出力
from .export_scene import MYADDON_OT_export_scene

#メニュー作成
from .my_menu import TOPBAR_MT_my_menu
from .my_menu import MYADDON_MT_sub_EnemySpawn
from .my_menu import MYADDON_MT_sub_StageObjectSpawn

# パネル ファイル名
from .file_name import OBJECT_PT_file_name

#オペレータ カスタムプロパティ['file_name']追加
from .add_filename import MYADDON_OT_add_filename

#
from .draw_collider import DrawCollider

#
from .add_collider import MYADDON_OT_add_collider

# パネル コライダー
from .collider import OBJECT_PT_collider

#
from .add_disabled import MYADDON_OT_add_disabled

#
from .disabled import OBJECT_PT_disabled

from .spawn import MYADDON_OT_spawn_import_symbol

from .spawn import MYADDON_OT_make_spawn_point

from .spawn import MYADDON_OT_create_player_spawn

from .spawn import MYADDON_OT_create_enemy_Soldier_spawn
from .spawn import MYADDON_OT_create_enemy_Turret_spawn
from .spawn import MYADDON_OT_create_enemy_Bomb_spawn
from .spawn import MYADDON_OT_create_boss_spawn

#ギミックオブジェクト
from .spawn import MYADDON_OT_create_check_point
from .spawn import MYADDON_OT_create_warp_gate
from .spawn import MYADDON_OT_create_goal
from .spawn import MYADDON_OT_create_move_ground

from .add_travel_route import MYADDON_OT_add_travel_route

from .travel_route import OBJECT_PT_travel_route

from .draw_travel_route import DrawTravelRoute

from .add_event_trigger import MYADDON_OT_add_eventTrigger
from .add_event_trigger import OBJECT_PT_move_eventTrigger
from .draw_event_trigger import DrawEventTrigger

classes = (
    MYADDON_OT_export_scene,
    TOPBAR_MT_my_menu,
    MYADDON_MT_sub_EnemySpawn,
    MYADDON_MT_sub_StageObjectSpawn,
    MYADDON_OT_add_filename,
    OBJECT_PT_file_name,
    MYADDON_OT_add_collider,
    OBJECT_PT_collider,
    MYADDON_OT_add_disabled,
    OBJECT_PT_disabled,
    MYADDON_OT_add_travel_route,
    OBJECT_PT_travel_route,
    MYADDON_OT_spawn_import_symbol,
    MYADDON_OT_make_spawn_point,
    MYADDON_OT_create_player_spawn,
    MYADDON_OT_create_enemy_Soldier_spawn,
    MYADDON_OT_create_enemy_Turret_spawn,
    MYADDON_OT_create_enemy_Bomb_spawn,
    MYADDON_OT_create_check_point,
    MYADDON_OT_create_warp_gate,
    MYADDON_OT_create_goal,
    MYADDON_OT_create_move_ground,
    MYADDON_OT_create_boss_spawn,
    MYADDON_OT_add_eventTrigger,
    OBJECT_PT_move_eventTrigger,

)

def register():
    #クラス登録
    for cls in classes:
        bpy.utils.register_class(cls)
       
    #メニューに項目を追加 
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)

    # 3Dビューに描画関数を追加
    DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(DrawCollider.draw_collider,(),"WINDOW","POST_VIEW")
    DrawTravelRoute.handle = bpy.types.SpaceView3D.draw_handler_add(DrawTravelRoute.draw_travel_route,(),"WINDOW","POST_VIEW")
    DrawEventTrigger.handle = bpy.types.SpaceView3D.draw_handler_add(DrawEventTrigger.draw_event_trigger,(),"WINDOW","POST_VIEW")

    print("レベルエディタが有効化されました")

def unregister():
    #メニューから項目を削除
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)

    # 3Dビューに描画関数を削除
    bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle,"WINDOW")
    bpy.types.SpaceView3D.draw_handler_remove(DrawTravelRoute.handle,"WINDOW")
    bpy.types.SpaceView3D.draw_handler_remove(DrawEventTrigger.handle,"WINDOW")

    #クラス登録
    for cls in classes:
        bpy.utils.unregister_class(cls)
    print("レベルエディタが無効化されました")
      

if __name__ == "__main__":
    register()