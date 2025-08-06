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

        # メモリ上にはおいておくがシーンでは除外する
        bpy.context.collection.objects.unlink(object)

        return {'FINISHED'}
    
    def execute(self, context):
        # Enemyオブジェクト読み込み
        self.load_obj("Enemy")
        # Playerオブジェクト読み込み
        self.load_obj("Player")
        
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

    names["Enemy"] = ("PrototypeEnemySpawn","EnemySpawn","enemy/enemy.obj")
    names["Player"] = ("PrototypePlayerSpawn","PlayerSpawn","player/player.obj")


##ここから下が怪しい

class MYADDON_OT_create_player_spawn(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_player_spawn"
    bl_label = "プレイヤー出現ポイントシンボルの作成"
    bl_description = "プレイヤー出現ポイントのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):

        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Player")

        return {'FINISHED'}
    

class MYADDON_OT_create_enemy_spawn(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_enemy_spawn"
    bl_label = "敵出現ポイントシンボルの作成"
    bl_description = "敵出現ポイントのシンボルを作成します"
    bl_options = {'REGISTER','UNDO'} 

    def execute(self, context):
        
        bpy.ops.myaddon.myaddon_ot_make_spawn_point('EXEC_DEFAULT',type="Enemy")

        return {'FINISHED'}