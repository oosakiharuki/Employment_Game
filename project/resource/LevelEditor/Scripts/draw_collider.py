import bpy
import mathutils
import gpu
import gpu_extras.batch
import copy

class DrawCollider:
    # 描画ハンドル
    handle = None

    def draw_collider():

        vertices = {"pos":[]}
        indices = []

        # 各頂点データ
        offsets = [
            [-0.5,-0.5,-0.5], #左下前 min
            [+0.5,-0.5,-0.5], #右下前
            [-0.5,+0.5,-0.5], #左上前
            [+0.5,+0.5,-0.5], #右上前
            [-0.5,-0.5,+0.5], #左下奥
            [+0.5,-0.5,+0.5], #右下奥
            [-0.5,+0.5,+0.5], #左上奥
            [+0.5,+0.5,+0.5], #右上奥 max
        ]

        size = [2,2,2] # サイズ

        for object in bpy.context.scene.objects:

            # コライダーのプロパティがないオブジェクトは描画しない(カメラ、ライトなど)
            if not "collider" in object:
                continue

            # 中心点、サイズの変更宣言
            center = mathutils.Vector((0,0,0))
            size = mathutils.Vector((2,2,2))

            # プロパティから値を取得
            center[0]=object["collider_center"][0]
            center[1]=object["collider_center"][1]
            center[2]=object["collider_center"][2]
            size[0]=object["collider_size"][0]
            size[1]=object["collider_size"][1]
            size[2]=object["collider_size"][2]

            start = len(vertices["pos"])

            for offset in offsets:
                # 中心座標
                pos = copy.copy(center)

                pos[0]+=offset[0]*size[0]
                pos[1]+=offset[1]*size[1]
                pos[2]+=offset[2]*size[2]

                #ローカル座標 -> ワールド座標
                pos = object.matrix_world @ pos
                # 頂点データ
                vertices['pos'].append(pos)
                
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
        batch = gpu_extras.batch.batch_for_shader(shader,"LINES",vertices,indices = indices)

        # シェーダのパラメータ
        color = [0.5, 1.0, 1.0, 1.0]
        shader.bind()
        shader.uniform_float("color", color)
        # 描画
        batch.draw(shader)
