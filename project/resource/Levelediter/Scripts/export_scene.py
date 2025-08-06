import bpy
import bpy_extras
import math

import json


#オペレータ シーン出力
class MYADDON_OT_export_scene(bpy.types.Operator,bpy_extras.io_utils.ExportHelper):
    bl_idname = "myaddon.myaddon_ot_export_scene"
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"

    filename_ext= ".json"
    
    def export_json(self):
        """JSO_N形式でファイル出力"""
        
        # 保持する情報をまとめるdist
        json_object_root = dict()
        
        # ノード名
        json_object_root["name"] = "scene"
        # オブジェcとリスト作成
        json_object_root["objects"] = list()

        #
        for object in bpy.context.scene.objects:
            #
            if(object.parent):
                continue
            #
            self.parse_scene_recursive_json(json_object_root["objects"], object, 0)

        # オブジェクトをJSON文字列にエンコード + 改行とインデント
        json_text = json.dumps(json_object_root,ensure_ascii=False,cls=json.JSONEncoder,indent=4)
        #コンソールに表示
        print(json_text)

        #ファイルをテキスト形式で書きだしようにオープン
        #スコープを抜けると自動的にクローズされる
        with open(self.filepath, "wt" , encoding="utf-8") as file:

            # ファイルに文字列を書き込む
            file.write(json_text)




    def parse_scene_recursive_json(self,data_parent,object,level):
        
        #
        json_object = dict()
        #オブジェクトの種類
        #json_object["type"] = object.type
        if "type" in object:
            json_object["type"] = object["type"]
        else:
            json_object["type"] = object.type
        #オブジェクト名
        json_object["name"] = object.name

        #Todo:
        #スケール
        trans,rot,scale = object.matrix_local.decompose()
        #回転(左手座標)
        rot = rot.to_euler()
        #ラジアンから度数法に
        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)
        #トランスフォーム
        transform = dict()
        transform["translation"] = (trans.x,trans.y,trans.z)
        transform["rotation"] = (rot.x,rot.y,rot.z)
        transform["scaling"] = (scale.x,scale.y,scale.z)
        #まとめる
        json_object["transform"] = transform

        #無効フラグ
        if "disabled" in object:
            json_object["disabled"] = object["disabled"]
        #オブジェクトの名前
        if "file_name" in object:
            json_object["file_name"] = object["file_name"]
        #コライダー
        if "collider" in object:
            collider = dict()
            collider["type"] = object["collider"]
            collider["center"] = object["collider_center"].to_list()
            collider["size"] = object["collider_size"].to_list()
            json_object["collider"] = collider

        #1個分のjsonオブジェクトを親オブジェクトに
        data_parent.append(json_object)

        #Todo:
        # 子ノードがあれば作る
        if len(object.children) > 0:
            json_object["children"] = list()

            #子ノードへ進む
            for child in object.children:
                self.parse_scene_recursive_json(json_object["children"], child, level + 1)











    def write_and_print(self,file,str):
        print(str)

        file.write(str)
        file.write('\n')

    def parse_scene_recursive(self,file,object,level):
        """"シーン解析用再起関数"""

        indent = ''
        for i in range(level):
            indent += "\t"

        self.write_and_print(file,indent + object.type)
        #trans = 平行移動 rot = 回転 scele = スケーリング
        trans,rot,scale = object.matrix_local.decompose()
        #Quatarnion -> Euler
        rot = rot.to_euler()
        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)

        #トランスフォーム情報
        self.write_and_print(file,indent + "Trans(%f,%f,%f)" % (trans.x,trans.y,trans.z))
        self.write_and_print(file,indent + "rot(%f,%f,%f)" % (rot.x,rot.y,rot.z))
        self.write_and_print(file,indent + "scale(%f,%f,%f)" % (scale.x,scale.y,scale.z))

        if "file_name" in object:
            self.write_and_print(file,indent + "N %s" % object["file_name"])
        
        if "collider" in object:
            self.write_and_print(file, indent + "C %s" % object["collider"])
            temp_str = indent + "CC %f %f %f"
            temp_str %= (object["collider_center"][0],object["collider_center"][1],object["collider_center"][2])
            self.write_and_print(file, temp_str)
            temp_str = indent + "CS %f %f %f"
            temp_str %= (object["collider_size"][0],object["collider_size"][1],object["collider_size"][2]) 
            self.write_and_print(file, temp_str)
        
        self.write_and_print(file,indent + 'END')
        self.write_and_print(file,'')

        for child in object.children:
            self.parse_scene_recursive(file,child,level + 1)



    def export(self):
        print("シーン情報出力開始... %r" % self.filepath)

        #ファイルをテキスト形式で書き出し用にオープン
        #スコープを抜けると自動的にクローズ
        with open(self.filepath,"wt") as file:

            #ファイルに文字列を書き込む
            file.write("SCENE\n")
                
            #シーン内のすべてのオブジェクト
            for object in bpy.context.scene.objects:
                #尾やオブジェクトの名前
                if (object.parent):
                    # self.write_and_print(file,"Parent : " + object.parent.name)
                    continue

                self.parse_scene_recursive(file,object,0)
            

            

    def execute(self,context):

        print("シーン情報をExportします")

        #ファイルに出力
        self.export_json()

        print("シーン情報をExportしました")
        self.report({'INFO'},"シーン情報をExportしました")

        return {'FINISHED'}
