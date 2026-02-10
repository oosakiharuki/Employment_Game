import bpy
import mathutils
from mathutils import Matrix
import gpu
import gpu_extras.batch
import copy

class DrawEventTrigger:

    handle = None

    def draw_event_trigger():

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
            if not "event_trigger" in object:
                continue

            center = mathutils.Vector((0,0,0))
            size = mathutils.Vector((2,2,2))

            center[0]=object["trigger_center"][0]
            center[1]=object["trigger_center"][1]
            center[2]=object["trigger_center"][2]
            size[0]=object["trigger_size"][0]
            size[1]=object["trigger_size"][1]
            size[2]=object["trigger_size"][2]

            start = len(vertices["pos"])

            for offset in offsets:

                translation_only_matrix = Matrix.Translation(object.location)

                pos = copy.copy(center)

                pos[0]+=offset[0]*size[0]
                pos[1]+=offset[1]*size[1]
                pos[2]+=offset[2]*size[2]

                #ローカル座標 -> ワールド座標 座標のみ
                pos = translation_only_matrix @ pos
                # 頂点データ
                vertices['pos'].append(pos)

                indices.append([start+0,start+1]) 
                indices.append([start+2,start+3]) 
                indices.append([start+0,start+2]) 
                indices.append([start+1,start+3])

                indices.append([start+4,start+5]) 
                indices.append([start+6,start+7]) 
                indices.append([start+4,start+6]) 
                indices.append([start+5,start+7]) 

                indices.append([start+0,start+4]) 
                indices.append([start+1,start+5]) 
                indices.append([start+2,start+6]) 
                indices.append([start+3,start+7]) 



        # ビルドインのシェーダを取得
        shader = gpu.shader.from_builtin("UNIFORM_COLOR")

        # パッチの作成 (シェーダ、トポロジー、頂点データ、インデックスデータ)
        batch = gpu_extras.batch.batch_for_shader(shader,"LINES",vertices,indices = indices)

        # シェーダのパラメータ
        color = [1.0, 1.0, 1.0, 1.0]
        shader.bind()
        shader.uniform_float("color", color)
        # 描画
        batch.draw(shader)