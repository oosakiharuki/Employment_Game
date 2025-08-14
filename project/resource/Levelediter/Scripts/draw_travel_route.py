import bpy
import mathutils
import gpu
import gpu_extras.batch
import copy

from mathutils import Matrix

class DrawTravelRoute:
    # 描画ハンドル
    handle = None

    def draw_travel_route():
        # 頂点データ
        vertices = {"pos":[]}
        # インデックスデータ
        indices = []


        for object in bpy.context.scene.objects:

            # コライダーのプロパティがないオブジェクトは描画しない(カメラ、ライトなど)
            if not "travel_route" in object:
                continue

            center = mathutils.Vector((0,0,0))
            start_point = mathutils.Vector((-2,0,0))
            end_point = mathutils.Vector((2,0,0))

            # プロパティから値を取得
            start_point[0]=object["start"][0]
            start_point[1]=object["start"][1]
            start_point[2]=object["start"][2]
            end_point[0]=object["end"][0]
            end_point[1]=object["end"][1]
            end_point[2]=object["end"][2]

            start = len(vertices["pos"])

            # location(座標)のみにする
            translation_only_matrix = Matrix.Translation(object.location)

            # p1
            start_pos = copy.copy(center)

            start_pos[0]+=start_point[0]
            start_pos[1]+=start_point[1]
            start_pos[2]+=start_point[2]

            start_pos = translation_only_matrix @ start_pos

            # 頂点データ
            vertices['pos'].append(start_pos)

            # p2
            end_pos = copy.copy(center) 

            end_pos[0]+=end_point[0]
            end_pos[1]+=end_point[1]
            end_pos[2]+=end_point[2]

            end_pos = translation_only_matrix @ end_pos
            
            # 頂点データ
            vertices['pos'].append(end_pos)

            # インデックスデータ
            indices.append([start+0,start+1])




        # ビルドインのシェーダを取得
        shader = gpu.shader.from_builtin("UNIFORM_COLOR")

        # パッチの作成 (シェーダ、トポロジー、頂点データ、インデックスデータ)
        batch = gpu_extras.batch.batch_for_shader(shader,"LINES",vertices,indices = indices)
        # シェーダのパラメータ
        color = [1.0, 0.5, 1.0, 1.0]
        shader.bind()
        shader.uniform_float("color", color)
        # 描画
        batch.draw(shader)

