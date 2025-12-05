import cv2
import numpy as np

def create_result_image(text, filename):
    # 画像サイズ
    width, height = 800, 200
    img = np.ones((height, width, 3), np.uint8) * 255  # 白背景

    # フォント設定
    font = cv2.FONT_HERSHEY_SIMPLEX
    font_scale = 3
    thickness = 5
    color = (0, 0, 0)  # 黒

    # テキストサイズ計算
    text_size = cv2.getTextSize(text, font, font_scale, thickness)[0]
    text_x = (width - text_size[0]) // 2
    text_y = (height + text_size[1]) // 2

    # テキスト描画
    cv2.putText(img, text, (text_x, text_y), font, font_scale, color, thickness)

    # 画像保存
    cv2.imwrite(filename, img)

# 画像作成
create_result_image("You Win!", "you_win.png")
create_result_image("You Lose!", "you_lose.png")