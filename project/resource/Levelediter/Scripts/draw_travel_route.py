import bpy
import mathutils
import gpu
import gpu_extras.batch
import copy

class DrawTravelRoute:
    # 描画ハンドル
    handle = None

    def draw_travel_route():
        vertices1 = {"pos1":[]}
        vertices2 = {"pos2":[]}
        indices = []

        for object in bpy.context.scene.objects:

            # コライダーのプロパティがないオブジェクトは描画しない(カメラ、ライトなど)
            if not "travel_route" in object:
                continue

            # 中心点、サイズの変更宣言
            start_point = mathutils.Vector((-2,0,0))
            end_point = mathutils.Vector((2,0,0))


            # プロパティから値を取得
            start_point[0]=object["start"][0]
            start_point[1]=object["start"][1]
            start_point[2]=object["start"][2]
            
            end_point[0]=object["end"][0]
            end_point[1]=object["end"][1]
            end_point[2]=object["end"][2]

            start = len(vertices1["pos1"])
            start = len(vertices2["pos2"])

            # 中心座標
            posS = copy.copy(start_point)
            posE = copy.copy(end_point)

            #ローカル座標 -> ワールド座標
            posS = object.matrix_world @ posS
            posE = object.matrix_world @ posE

            # 頂点データ
            vertices1['pos1'].append(posS)
            vertices2['pos2'].append(posE)
                
            # インテックスデータ
            #前面
            indices.append([start+0,start+1])
            indices.append([start+2,start+3])
            indices.append([start+0,start+2])
            indices.append([start+1,start+3])
            #奥面
            indices.append([start+4,start+5])
            indices.append([start+6,start+7])
            indices.append([start+4,start+6])
            indices.append([start+5,start+7])
            #手前と奥
            indices.append([start+0,start+4])
            indices.append([start+1,start+5])
            indices.append([start+2,start+6])
            indices.append([start+3,start+7])


        # ビルドインのシェーダを取得
        shader = gpu.shader.from_builtin("UNIFORM_COLOR")

        # パッチの作成 (シェーダ、トポロジー、頂点データ、インデックスデータ)
        batch1 = gpu_extras.batch.batch_for_shader(shader,"LINES",vertices1,indices = indices)
        batch2 = gpu_extras.batch.batch_for_shader(shader,"LINES",vertices2,indices = indices)

        # シェーダのパラメータ
        color = [0.5, 1.0, 1.0, 1.0]
        shader.bind()
        shader.uniform_float("color", color)
        # 描画
        batch1.draw(shader)
        batch2.draw(shader)
