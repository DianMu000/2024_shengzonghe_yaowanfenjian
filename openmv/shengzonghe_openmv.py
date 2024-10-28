#For Game GongKe
#2024年10月21日19:57:35

#库引用
import sensor, image, time#基本库
import os#存Temp文件库
from pyb import UART#串口库
import ubinascii#转16进制库
from pyb import LED
#import statistics
#*****************************

#数据库
#颜色识别后的不正确数据
data = []
#正确数据
data_one = []
#圆形识别的过渡数据
data_more = []
#串口接收的数据
data_uart = [0x00, 0x00, 0x00]
#转换坐标系参数
K1 =  150/152        #162  /160 #长度转换系数  横   实际值 / 像素值
K2 =  110/112                 #110 / 112 #宽度转换系数  竖0 0.98435489
offset1 = 0
offset2 = 0
#P中 = [ 200, 100]           #摄像头在主坐标系的位置（mm）
#最终坐标（主）
data_A = []

曝光阈值 = 40000
曝光计数 = 0
#0  +  15     + (13/K2)
#120 -5 -(13/K2)
#0   +  20    +(13/K1)
#160 -0 -(13/K1)



上边界 =                    65.0 - 7.0 - 12.5
下边界 =                    -65.0 + 12.5 + 12.0
左边界 =                    -85.0 + 5.0 + 12.5
右边界 =                     85.0 - 12.5
#*****************************


#烧录步骤 ： 工具-> 把打开的脚本保存到Openmv Cam 中




#感光元件 基本参数设置
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking  #自动增益效果
sensor.set_auto_whitebal(False)  # must be turned off for color tracking #白平衡
sensor.set_auto_exposure(False , exposure_us = 曝光阈值)#    曝光值 ,exposure_us=9000)30000
clock = time.clock()#放时钟类
#*****************************

#颜色识别
#颜色识别参数（LAB）
thresholds = [
    (10, 90, -5, 127, 5, 127),  # generic_red_thresholds
    (10, 98, -128, -15, -21, 18),  # generic_green_thresholds
    (10, 98, -38, 37, -128, -12),  # generic_blue_thresholds
    (80,100, -15, 15, -15, 15),      # generic_white_thresholds
]#   LMIN,LMAX,AMIN,AMAX,BMIN,BMAX
def colorTrack( Img, More=[0,0,160,120], color=255,data_color=0):
    #sensor.reset()
    #sensor.set_pixformat(sensor.RGB565)
    #sensor.set_framesize(sensor.QQVGA)
    #sensor.skip_frames(time = 2000)
    #sensor.set_auto_gain(False)  # must be turned off for color tracking  #自动增益效果
    #sensor.set_auto_whitebal(False)  # must be turned off for color tracking #白平衡
    #sensor.set_auto_exposure(True)#    曝光值 ,exposure_us=9000)
    #clock = time.clock()#放时钟类
    #current_exposure_time_in_microseconds = sensor.get_exposure_us()#获取曝光值
    #clock.tick()
    img = Img

    if data_color != 0 and data_color != 1 and data_color != 2 :
        data_color = 0

    for blob in img.find_blobs([thresholds[data_color]],pixels_threshold=100, area_threshold=100):
        # These values are stable all the time.
        img.draw_rectangle(blob.rect(),color = (255,0 ,0))
        #img.draw_cross(blob.cx(), blob.cy(),color=(255,color,color))
        #img.draw_cross(int(blob.cx()-(blob.w()/2)), int(blob.cy()-(blob.h()/2)))
        if blob.w()*blob.h() > 20*20:
            data.append(blob)
        else:
            data_one.append(blob)
            print(blob)
#*****************************

#圆形识别（在颜色识别后）（放数据到data_one）
def circleTrack(More,Img):
    #clock.tick()
    img = Img
    #img = sensor.snapshot().lens_corr(1.8)
    for c in img.find_circles(roi=More,threshold = 2000, x_margin = 10, y_margin = 10, r_margin = 10,r_min = 10, r_max = 100, r_step = 2):
        area = (c.x()-c.r(), c.y()-c.r(), 2*c.r(), 2*c.r())
        #area为识别到的圆的区域，即圆的外接矩形框    #
        #(0,100,0,120,0,120)是红色的阈值，所以当区域内的众数（也就是最多的颜色），范围在这个阈值内，就说明是红色的圆。
        #l_mode()，a_mode()，b_mode()是L通道，A通道，B通道的众数。
        img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))#识别到的红色圆形用红色的圆框出来
        if c.r()*c.r()*3.14 < 22*22 and c.r()*c.r()*3.14 > 5*5:
            img.draw_rectangle(area, color = (255, 0, 0))
            data_one.append(c)
#*****************************

#串口
State = 0

uart = UART(1, 115200 , timeout_char=1000)#P0 P1 为1号口，
#Openmv接收STM32的串口信号（16进制-->str）
def STM32_2_MV():
    if uart.any():
        data = uart.read(3)
        data_check = ubinascii.hexlify(data).decode('utf-8')

        #print(data_check)
        #切割
        data_tail = data_check[4:6]
        data_color = data_check[2:4]
        data_head = data_check[0:2]

        data_uart = [ int(data_head), int(data_color), data_tail]

        data_tail = 0
        data_color = 0
        data_head = 0


        #print(data_head)
        #print(data_need)
        #print(data_tail)
#*****************************
#Openmv串口与STM32（16进制）
def MV_2_STM32(x, y):
    x坐标高8位 = (x >> 8) & 0xFF
    x坐标低8位 = x & 0xFF
    y坐标高8位 = (y >> 8) & 0xFF
    y坐标低8位 = y & 0xFF
    t = [0x60, State, x坐标高8位, x坐标低8位, y坐标高8位, y坐标低8位,0x6b]
    blue_led.on()
    time.sleep_ms(100)
    blue_led.off()
    uart.write(bytes(t))
    print(t)
#*****************************
#*****************************

#最后的颜色辨别，确保所识别的是该颜色
def judgeColor(Img, x, y):
    img = Img
    #ROI = (x-5, y-5, 10, 10)
    #color = img.get_statistics(roi=ROI)
    #color_L = color.l_mean()
    #color_A = color.a_mean()
    #color_B = color.b_mean()
    #color_LAB = [color_L, color_A, color_B]
    #print(color_LAB)
    #if color_A > 0 and color_B > 0 :
    #    return 0
    #elif color_A < 0 and color_B > -32:
    #    return 1
    #elif color_A > -64 and color_B < 0 :
    #    return 2
    rgb = []

    point0 = [x,y]
    point1 = [x-2,y]
    point2 = [x,y-2]
    point3 = [x+2,y]
    point4 = [x,y+2]
    point = [point0,point1,point2,point3,point4]
    for p in point :
        rgbcolor = img.get_pixel(p[0], p[1])
        print(rgbcolor)
        if rgbcolor[0] > rgbcolor[1] and rgbcolor[0] > rgbcolor[2] :
            rgb.append(0)
        elif rgbcolor[1] > rgbcolor[0] and rgbcolor[1] > rgbcolor[2] :
            rgb.append(1)
        elif rgbcolor[2] > rgbcolor[0] and rgbcolor[2] > rgbcolor[1] :
            rgb.append(2)
        else:
            rgb.append(3)
    cr = 0
    cg = 0
    cb = 0
    co = 0
    print(rgb)
    print('rgb:')
    print(point0)
    for num in rgb :
        if num == 0 :
            cr += 1
        if num == 1 :
            cg += 1
        if num == 2 :
            cb += 1
        if num == 3 :
            co += 1
    rgb=[]
    if cr >= cg and cr >= cb and cr >= co :
        return 0
    elif cg >= cr and cg >= cb and cg >= co :
        return 1
    elif cb >= cg and cb >= cb and cb >= co :
        return 2
    else:
        return 3




#*****************************
#瓶子定位
#def bottleTrack(Img):
#    img = Img
#    for blob in img.find_blobs([thresholds[3]],pixels_threshold=100, area_threshold=100):
#        # These values are stable all the time.
#        img.draw_rectangle(blob.rect())
#        img.draw_cross(blob.cx(), blob.cy(),color=(255,0,0))
#        img.draw_cross(int(blob.cx()-(blob.w()/2)), int(blob.cy()-(blob.h()/2)))
#*****************************

#动态检测
#if not "temp" in os.listdir():
#    os.mkdir("temp")  # 新建一个新的文件夹
#def Dynamic_or_Static():
#    diff = 10  #给它10帧
#    error = 0
#    right = 0
#    while diff:
#        img = sensor.snapshot().lens_corr(1.8)
#        img.difference("temp/bg.bmp")
#        stats = img.statistics()
#        # state[5]是照明颜色通道的最大值。当整个图像的最大光照高于20时
#        # 触发下面的代码。
#        # 照明差异最大值应该为零。
#        if stats[5] > 20 :
#            error += 1
#        else :
#            right += 1
#        diff -= 1
#    if error > right :
#        return 1
#    else :
#        return 0
#*****************************
#*****************************

#摄像头坐标系转主坐标系 （p→mm）
def zhuanhuan(x,y):
    #x0_y = P中[0] + ( 60 - y ) * K2
    #y0_x = P中[1] + ( 80 - x ) * K1
    #if y <= 下边界 :
    #    y = 下边界
    #elif y >= 上边界 :
    #    y = 上边界
    #
    #if x <= 左边界 :
    #    x = 左边界
    #elif x >= 右边界 :
    #    x = 右边界
    x0_y = (60 - y) * K2 + offset2
    y0_x = (80 - x) * K1 * (-1) + offset1
    return [x0_y,y0_x]
#*****************************
red_led   = LED(1)
blue_led  = LED(3)
color_mine = [0,1,2]
while(True):
    clock.tick()
    State = 0
    data = []
    data_one = []
    data_A = []
    data_uart = [0x00, 0x00, 0x00]
    A = []
    #m = zhuanhuan(50,50)
    #MV_2_STM32(int(m[0]),int(m[1]))
    #print(int(m[0]),int(m[1]))
    img = sensor.snapshot().lens_corr(1.42)
    #img.draw_rectangle((78, 59, 2, 2), color=(255,255,255))
    #等待串口，未等到会一直循环以上代码                                等串口
    #STM32_2_MV()
    if uart.any():
        data_s = uart.read(1)
        data_check = ubinascii.hexlify(data_s).decode('utf-8')
        data_uart[0] = data_check

        if data_uart[0] == '60' :
            data_uart[0] = int(data_check)
            if uart.any():
                data_s = uart.read(2)
                data_check = ubinascii.hexlify(data_s).decode('utf-8')

                data_tail = data_check[2:4]
                data_color = data_check[0:2]

                if data_tail != '6b' :
                    continue
                data_uart[1] = data_color
                data_uart[2] = data_tail
        else :
            continue
    if data_uart[0] != 60 :
        continue
    if data_uart[2] != '6b' :
        continue

    #颜色识别
    print(int(data_uart[1]))
    colorTrack( img, color=255, data_color = int(data_uart[1]))
    if len(data) == 0 :
        print("no data")
        曝光计数 = 曝光计数 + 1
    else :
        for more in data :
            data_more.append(more[0] - 10)
            data_more.append(more[1] - 10)
            data_more.append(more[2] + 20)
            data_more.append(more[3] + 20)
            print(data_more)
            #print(tuple(data_more))
            #圆形识别
            circleTrack(tuple(data_more),Img = img)
            #colorTrack(img,tuple(data_more),0)
            data_more = []
    #print(data)
    #发送坐标
    data_A = [0,0]
    if len(data_one) > 0:
        A = data_one[0]
        if judgeColor(img, int(A[0]+A[2]*0.5), int(A[1]+A[3]*0.5)) != int(data_uart[1]) :
            continue
            print(A)
        else :
            State = 1
            red_led.on()
            time.sleep_ms(100)
            red_led.off()
            data_A = zhuanhuan(A[0],A[1])
            if len(data_A) == 0 :
                data_A = [0,0]
    #print(clock.fps())
    if data_A[0] <= 下边界 :
        data_A[0] = 下边界
    elif data_A[0] >= 上边界 :
        data_A[0] = 上边界

    if data_A[1] <= 左边界 :
        data_A[1] = 左边界
    elif data_A[1] >= 右边界 :
        data_A[1] = 右边界



    MV_2_STM32(int(data_A[0]),int(data_A[1]))
    if 曝光计数 == 4 :
        曝光阈值 = 曝光阈值 + 5000
    #if 曝光阈值 > 20000 :
    #    曝光阈值 = 20000
    #数据清零
    State = 1
    data = []
    data_one = []
    data_A = []
    data_uart = [0x00, 0x00, 0x00]
    A = []
    sensor.set_auto_exposure(False , exposure_us = 曝光阈值)#    曝光值 ,exposure_us=9000)30000
