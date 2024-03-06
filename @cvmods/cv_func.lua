--[[
    cvmoduleスクリプト用関数モジュール | 最終更新 : 2024/02/24
    This scripts are written in Shift-JIS.
]]

----------------------------------------------------------------------------------------------------------------------------------
-- メディアンブラー
----------------------------------------------------------------------------------------------------------------------------------
md_flt = function(slider0)
    local b = require("cvmods")
    b.medianblur(slider0)
end

----------------------------------------------------------------------------------------------------------------------------------
-- ボックスブラー
----------------------------------------------------------------------------------------------------------------------------------
bx_flt = function(slider0,slider1,slider2,switch0)
    local b = require("cvmods")
    b.boxblur(slider0,slider1,switch0,slider2)
end

----------------------------------------------------------------------------------------------------------------------------------
-- ガウシアンブラー
----------------------------------------------------------------------------------------------------------------------------------
gs_flt = function(slider0,slider1,slider2,slider3,border)
    local b = require("cvmods")
    if (slider0+slider1+slider2+slider3 == 0) then
        print("すべてのパラメータを0にすることはできません")
        return
    end
    b.gaussianblur(slider0,slider1,slider2,slider3,border)
end

----------------------------------------------------------------------------------------------------------------------------------
-- バイラテラルフィルター
----------------------------------------------------------------------------------------------------------------------------------
bl_flt = function(slider0,slider1,slider2,slider3,switch0)
    local b = require("cvmods")
    b.bilateralfilter(slider0,slider1,slider2,slider3,switch0)
end

----------------------------------------------------------------------------------------------------------------------------------
-- リサイズ
----------------------------------------------------------------------------------------------------------------------------------
im_rsz = function(slider0,slider1,slider2,slider3,switch0,rw,rh,exp)
    if (slider0*slider1*slider2 <= 0) and (not switch0) then
        print("サイズ指定を有効化するか、0より大きい値を入力してください")
        return
    end

    local cw = slider0 * slider1 / 10000
    local ch = slider0 * slider2 / 10000
    local b = require("cvmods")
    b.resizefilter(rw, rh, cw, ch, slider3, switch0, mov)
end

----------------------------------------------------------------------------------------------------------------------------------
-- リサンプリング
----------------------------------------------------------------------------------------------------------------------------------
rs_smp = function(slider0,slider1,slider2,mov)
    local b = require("cvmods")
    b.resample(slider0,slider0,slider2,slider1,mov)
end

----------------------------------------------------------------------------------------------------------------------------------
-- エッジ抽出(Canny)
----------------------------------------------------------------------------------------------------------------------------------
cn_flt = function(slider0,slider1,slider2,slider3,switch0,black)
    local b = require("cvmods")
    if switch0 then
        b.cannyRGB(slider1,slider2,slider0,slider3,black)
    else
        b.cannyfilter(slider1,slider2,slider0,slider3,black)
    end
end

----------------------------------------------------------------------------------------------------------------------------------
-- エッジ抽出(laplacian)
----------------------------------------------------------------------------------------------------------------------------------
ll_flt = function(slider0,slider1,slider2,slider3,switch0,black)
    local b = require("cvmods")
    local scale = slider1/100
    local delta = slider2/100
    if switch0 then
        b.laplacianRGB(slider0,black,slider3,scale,delta)
    else
        b.laplacianfilter(slider0,black,slider3,scale,delta)
    end
end

----------------------------------------------------------------------------------------------------------------------------------
-- エッジ抽出(sobel)
----------------------------------------------------------------------------------------------------------------------------------
sb_flt = function(slider0,slider1,slider2,slider3,switch0,scale,delta,black)
    local b = require("cvmods")
    local dx = slider1/100
    local dy = slider2/100
    if switch0 then
        b.sobelRGB(slider0,black,dx,dy,slider3,scale,delta)
    else
        b.sobelfilter(slider0,black,dx,dy,slider3,scale,delta)
    end
end

----------------------------------------------------------------------------------------------------------------------------------
-- モルフォロジー変換
----------------------------------------------------------------------------------------------------------------------------------
de_mrp = function(slider0,slider1,slider2,slider3,iter,alp)
    local b = require("cvmods")
    b.morphologyfilter(slider0, slider2, slider1, iter, slider3, alp)
end

cl_mrp = function(slider0,slider1,slider2,slider3,ker,iter,bor,alp)
    local b = require("cvmods")
    b.morphologyRGB(slider2, slider1, slider0, ker, slider3, iter, bor, alp)
end

----------------------------------------------------------------------------------------------------------------------------------
-- 二値化
----------------------------------------------------------------------------------------------------------------------------------
th_flt = function(slider0,slider1,slider2)
    local b = require("cvmods")
    b.thres(slider0,slider1,slider2)
end

th_adp = function(slider0,slider1,slider2,slider3,inv)
    local b = require("cvmods")
    b.adpthres(slider2,slider0,slider1,inv,slider3)
end

----------------------------------------------------------------------------------------------------------------------------------
-- ディフュージョン
----------------------------------------------------------------------------------------------------------------------------------
df_flt = function(slider0,slider1,slider2,slider3,sigma)
    local b = require("cvmods")
    b.diffusion(slider3,slider0,slider1,slider2,sigma)
end

----------------------------------------------------------------------------------------------------------------------------------
-- ノイズ
----------------------------------------------------------------------------------------------------------------------------------
nz_flt = function(slider0,slider1,slider2)
    local b = require("cvmods")
    b.noise(slider0,slider1,slider2)
end

----------------------------------------------------------------------------------------------------------------------------------
-- 画像ブラー
----------------------------------------------------------------------------------------------------------------------------------
fg_blr = function(slider0,slider1,filename)
    local path = string.gsub(filename, "\\", "/") 
    local b = require("cvmods")
    b.fig_blur(slider0,slider1,path)
end

----------------------------------------------------------------------------------------------------------------------------------
-- ドロステ効果
----------------------------------------------------------------------------------------------------------------------------------
--[[開発中]]

----------------------------------------------------------------------------------------------------------------------------------
-- メビウス変換
----------------------------------------------------------------------------------------------------------------------------------
mb_trf = function(slider0,slider1,slider2,slider3,switch0,mb_a,mb_b,mb_c,mb_d,nw,nh)
    
    local ca={}
    local cb={}
    local cc={}
    local cd={}
    local istabel = type(ca)
    local isn = type(0)

    local tya = type(mb_a)
    if (tya == istabel) then
        ca = mb_a
    elseif (tya == isn) then
        ca = {slider0, mb_a}
    else
        ca = {slider0, 0}
    end

    local tyb = type(mb_b)
    if (tyb == istabel) then
        cb = mb_b
    elseif (tyb == isn) then
        cb = {slider1, mb_b}
    else
        cb = {slider1, 0}
    end

    local tyc = type(mb_c)
    if (tyc == istabel) then
        cc = mb_c
    elseif (tyc == isn) then
        cc = {slider2, mb_c}
    else
        cc = {slider2, 0}
    end

    local tyd = type(mb_d)
    if (tyd == istabel) then
        cd = mb_d
    elseif (tyc == isn) then
        cd = {slider3, mb_d}
    else
        cd = {slider3, 0}
    end

    --[[
        テーブルの受け渡し後は入れ子になっている順に変数が格納されていきます
        テーブルのキーは数字(key未指定)->文字(key指定)の順です。ただし、文字内、数字内では先に到達する方が先の要素となります。
        例 要素はすべて std::vector<double> 型
        ca,cb,cc,cdはすべて {?, ?} の構造である
        coef = { 1,0, ff={{-1},{-5,-7},{-110,59}}, ca,cb,cc,cd, c={{{1,4,55,77},{5,-100, 59}},{-480}}, {{1,4},{5,7}}, {{{{{712}}}}}, -1 }
        -> vec[0] = {1, 0, -1}
        -> vec[1] = {ca[0], ca[1], cb[0], cb[1], cc[0], cc[1], cd[0], cd[0]}
        -> vec[2] = {1, 4, 5, 7, -1, -5, -7, -110, 59, -480}
        -> vec[3] = {1, 4, 55, 77, 5,-100, 59}
        -> vec[4] = {}
        -> vec[5] = {712}
        vec[6]以下はすべて {}
    ]]
    coef = { 4, ca, {cb}, {{cc}}, {{{cd}}} } 
    local b = require("cvmods")
    
    b.moebius(switch0,nw,nh)
    coef = nil
end

----------------------------------------------------------------------------------------------------------------------------------
-- Meanshiftフィルタ
----------------------------------------------------------------------------------------------------------------------------------
ms_flt = function(slider0,slider1,slider2)
    local b = require("cvmods")
    b.meanshift(slider0,slider1,slider2)
end

----------------------------------------------------------------------------------------------------------------------------------
-- ボロノイ変換
----------------------------------------------------------------------------------------------------------------------------------
vn_flt = function(slider0,slider1,slider2,draw,lr,lg,lb,thick)
    local b = require("cvmods")
    b.voronoi(slider0,slider1,draw,lr,lg,lb,thick,slider2)
end

----------------------------------------------------------------------------------------------------------------------------------
-- ドロネー変換
----------------------------------------------------------------------------------------------------------------------------------
bn_flt = function(slider0,slider1,slider2,draw,lr,lg,lb,thick)
    local b = require("cvmods")
    b.delaunay(slider0,slider1,draw,lr,lg,lb,thick,slider2)
end

----------------------------------------------------------------------------------------------------------------------------------
-- PencilSketch
----------------------------------------------------------------------------------------------------------------------------------
pc_skc = function(slider0,slider1,slider2,switch0)
    local b = require("cvmods")
    b.pencilskc(slider0,slider1,slider2,switch0)
end

----------------------------------------------------------------------------------------------------------------------------------
-- Stylization
----------------------------------------------------------------------------------------------------------------------------------
st_flt = function(slider0,slider1)
    local b = require("cvmods")
    b.stylize(slider0,slider1)
end

----------------------------------------------------------------------------------------------------------------------------------
-- 鮮明化フィルタ
----------------------------------------------------------------------------------------------------------------------------------
dt_ehc = function(slider0,slider1)
    local b = require("cvmods")
    b.det_enhance(slider0,slider1)
end

----------------------------------------------------------------------------------------------------------------------------------
-- エッジ保存フィルタ
----------------------------------------------------------------------------------------------------------------------------------
ed_prs = function(slider0,slider1,slider2)
    local b = require("cvmods")
    b.edgePreserve(slider0,slider1)
end

----------------------------------------------------------------------------------------------------------------------------------
-- カラーマップ
----------------------------------------------------------------------------------------------------------------------------------
cl_map = function(slider0,slider1,slider2)
    local b = require("cvmods")
    b.color_map(slider0,slider1,slider2)
end

----------------------------------------------------------------------------------------------------------------------------------
-- 油絵風
----------------------------------------------------------------------------------------------------------------------------------
op_flt = function(slider0, slider1)
    local b = require("cvmods")
    b.oilpaint(slider0,slider1)
end

----------------------------------------------------------------------------------------------------------------------------------
-- ヒストグラム平坦化
----------------------------------------------------------------------------------------------------------------------------------
eq_hst = function(slider0,slider1,slider2,slider3,switch0)
    local b = require("cvmods")
    b.hist_eq(slider0,slider1,slider2,slider3,switch0)
end

----------------------------------------------------------------------------------------------------------------------------------
-- チャンネル入れ替え
----------------------------------------------------------------------------------------------------------------------------------
ch_rpl = function(slider0,slider1,slider2,slider3)
    local b = require("cvmods")
    b.ch_chg(slider2,slider1,slider0,slider3)
end

----------------------------------------------------------------------------------------------------------------------------------
-- 画像読み込み
----------------------------------------------------------------------------------------------------------------------------------
im_red = function(slider0,slider1,slider2,filename)
    local path = string.gsub(filename, "\\", "/") 
    local tmp = string.sub(path, -3)
    if tmp == "svg" then
        slider0 = -2
        slider1 = slider1 * 10
        slider2 = slider2 * 10
    end

    local b = require("cvmods")
    b.img_read(path,slider0,slider1,slider2)
end

--[[
    svg参照
    - https://github.com/sammycage/lunasvg
    - https://www.exceedsystem.net/2021/06/09/how-to-resolve-build-errors-when-including-svgpp-in-visual-cpp/
    - https://github.com/svgpp/svgpp?tab=readme-ov-file
]]

----------------------------------------------------------------------------------------------------------------------------------
-- 動画読み込み
----------------------------------------------------------------------------------------------------------------------------------
mv_red = function(slider0,slider1,slider2,slider3,switch0,frame)
    local idx = 0
    if slider3==0 then
        idx = -1
    elseif slider3==1 then
        idx = -2
    else
        idx = frame
    end

    local b = require("cvmods")
    b.mov_read(idx,slider1,slider2,slider0,switch0)
end