import bpy
import os
import bpy.ops

# 出現ポイントのシンボルの読み込み
class MYADDON_OT_spawn_import_symbol(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_spawn_import_symbol"
    bl_label = "出現ポイントシンボルImport"
    bl_description = "出現ポイントのシンボルをImportします"
    
    #prototype_object_name = "ProttypePlayerSpawn"
    #object_name = "PlayerSpawn"

    def load_obj(self, type):
        #print("出現ポイント_import")
        
        # オブジェクトが重複しない
        spawn_object = bpy.data.objects.get(SpawnNames.names[type][SpawnNames.PROTOTYPE])
        # オブジェクトが空じゃないときキャンセル
        if spawn_object is not None:
            return {'CANCELLED'}
        
        # スクリプトが配置されているディレクトリ
        addon_directory = os.path.dirname(__file__)
        # モデルのファイルパス
        relative_path = SpawnNames.names[type][SpawnNames.FILENAME]
        # モデルファイルのフルパス
        full_path = os.path.join(addon_directory, relative_path)

        # モデルファイルの読み込み
        # object import
        bpy.ops.wm.obj_import('EXEC_DEFAULT',filepath=full_path,display_type='THUMBNAIL',forward_axis='Z',up_axis='Y')
        
        # 回転を適用
        bpy.ops.object.transform_apply(location=False,rotation=True,scale=False,properties=False,isolate_users=False)
        
        # アクティブなオブジェクトの取得
        object = bpy.context.active_object
        # オブジェクト名を変更
        object.name = SpawnNames.names[type][SpawnNames.PROTOTYPE]
        # オブジェクトの種類を設定
        object["type"] = SpawnNames.names[type][SpawnNames.INSTANCE]

        #EnemySpawnなら
        if "EnemySpawn" in object["type"]:
            object["EnemyName"] = type
        
        if "StageObjectSpawn" in object["type"]:
            object["StageObjectName"] = type

        # メモリ上にはおいておくがシーンでは除外する
        bpy.context.collection.objects.unlink(object)

        return {'FINISHED'}
    
    def execute(self, context):
        # Enemyオブジェクト読み込み
        self.load_obj("Soldier")
        self.load_obj("Turret")
        self.load_obj("Bomb")
        # Playerオブジェクト読み込み
        self.load_obj("Player")
        # Checkpointオブジェクト読み込み
        self.load_obj("Checkpoint")
        # Checkpointオブジェクト読み込み
        self.load_obj("WarpGate")
        # Goalオブジェクト読み込み
        self.load_obj("Goal")
        # MoveGroundオブジェクト読み込み
        self.load_obj("MoveGround")
        #Bossオブジェクト読み込み
        self.load_obj("Boss")
        return {'FINISHED'}

    


# シンボルの作成 #spawn_create_Symbol
class MYADDON_OT_make_spawn_point(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_make_spawn_point"
    bl_label = "出現ポイントのシンボルを作成"
    bl_description = "出現ポイントのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 
    
    # プロパティ　
    type: bpy.props.StringProperty(name="Type", default="Player")  #黄色線は仕方がない

    def execute(self, context):
        #読み込み済みのコピー完了オブジェクトを検索
        spawn_object = bpy.data.objects.get(SpawnNames.names[self.type][SpawnNames.PROTOTYPE])

        #読み込んでいない場合
        if spawn_object is None:
            # 読み込みオペレータ実行 / class MYADDON_OT_spawn_import_symbol
            bpy.ops.myaddon.myaddon_ot_spawn_import_symbol('EXEC_DEFAULT')
            # 再検索
            spawn_object = bpy.data.objects.get(SpawnNames.names[self.type][SpawnNames.PROTOTYPE])

        print("出現ポイントのシンボルを作成します")
        #Blender選択解除
        bpy.ops.object.select_all(action='DESELECT')

        #非表示オブジェクトを複製
        object = spawn_object.copy()

        #複製したオブジェクトを現在のシーンに出現させる
        bpy.context.collection.objects.link(object)

        #オブジェクト名の変更
        object.name = SpawnNames.names[self.type][SpawnNames.INSTANCE]

        return {'FINISHED'}


class SpawnNames():
    PROTOTYPE = 0 # プロトタイプのオブジェクト名
    INSTANCE = 1  # 量産時のオブジェクト名
    FILENAME = 2  # リソースファイル名

    names = {}

    names["Player"] = ("PrototypePlayerSpawn","PlayerSpawn","player/player.obj")

    names["Soldier"] = ("PrototypeEnemySpawnA","EnemySpawn","enemies/soldier/enemy.obj")
    names["Turret"] = ("PrototypeEnemySpawnB","EnemySpawn","enemies/turret/cannon.obj")
    names["Bomb"] = ("PrototypeEnemySpawnC","EnemySpawn","enemies/bomb/enemy_bomb.obj")

    names["Checkpoint"] = ("PrototypeCheckpoint","StageObjectSpawn","stageObjects/checkpoint/checkpoint.obj")
    names["WarpGate"] = ("PrototypeWorpGate","StageObjectSpawn","stageObjects/warpGate/warpGate.obj")
    names["Goal"] = ("PrototypeGoal","StageObjectSpawn","stageObjects/goal/goal.obj")
    names["MoveGround"] = ("PrototypeMoveGround","StageObjectSpawn","stageObjects/moveGround/moveGround.obj")

    names["Boss"] = ("PrototypeBoss","BossSpawn","Boss/Boss.obj")

class MYADDON_OT_create_player_spawn(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_player_spawn"
    bl_label = "プレイヤー出現ポイントシンボルの作成"
    bl_description = "プレイヤー出現ポイントのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):

        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Player")

        return {'FINISHED'}
    

class MYADDON_OT_create_enemy_Soldier_spawn(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_enemy_soldier_spawn"
    bl_label = "敵出現ポイントシンボル-soldier"
    bl_description = "敵[soldier]出現ポイントシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):
        
        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Soldier")

        return {'FINISHED'}
    
class MYADDON_OT_create_enemy_Turret_spawn(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_enemy_turret_spawn"
    bl_label = "敵出現ポイントシンボル-turret"
    bl_description = "敵[turret]出現ポイントのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):
        
        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Turret")
        
        return {'FINISHED'}

class MYADDON_OT_create_enemy_Bomb_spawn(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_enemy_bomb_spawn"
    bl_label = "敵出現ポイントシンボル-bomb"
    bl_description = "敵[bomb]出現ポイントのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):
        
        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Bomb")
        
        return {'FINISHED'}
    
class MYADDON_OT_create_check_point(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_check_point"
    bl_label = "チェックポイントシンボルの作成"
    bl_description = "チェックポイントのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):
        
        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Checkpoint")

        return {'FINISHED'}
    
class MYADDON_OT_create_warp_gate(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_warp_gate"
    bl_label = "ワープゲートポイントシンボルの作成"
    bl_description = "次のステージに切り替えるワープゲートのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):
        
        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="WarpGate")

        return {'FINISHED'}
        
        
class MYADDON_OT_create_goal(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_goal"
    bl_label = "ゴールポイントシンボルの作成"
    bl_description = "ゴールのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):
        
        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Goal")

        return {'FINISHED'}

class MYADDON_OT_create_boss_spawn(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_boss_spawn"
    bl_label = "ボス出現ポイントシンボル"
    bl_description = "ボス出現ポイントのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):
        
        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Boss")
        
        return {'FINISHED'}
    
class MYADDON_OT_create_move_ground(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_move_ground"
    bl_label = "動く足場ポイントシンボル"
    bl_description = "動く足場のポイントシンボルを作成します"
    bl_options = {'REGISTER','UNDO'}

    def execute(self,context):

        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="MoveGround")

        return {'FINISHED'}

