// pch.h: プリコンパイル済みヘッダー ファイルです。
// 次のファイルは、その後のビルドのビルド パフォーマンスを向上させるため 1 回だけコンパイルされます。
// コード補完や多くのコード参照機能などの IntelliSense パフォーマンスにも影響します。
// ただし、ここに一覧表示されているファイルは、ビルド間でいずれかが更新されると、すべてが再コンパイルされます。
// 頻繁に更新するファイルをここに追加しないでください。追加すると、パフォーマンス上の利点がなくなります。

#ifndef PCH_H
#define PCH_H

// プリコンパイルするヘッダーをここに追加します
#include "framework.h"
#include <mmeapi.h>
#include <aviutl.hpp>
#include <exedit.hpp>
#include <afxwin.h>
#include <windows.h>
#include <afxdlgs.h>
#include <ppl.h>
#include <psapi.h>
#include <iostream>
#include <any>
#include <string>
#include <memory>
#include <array>
#include <vector>
#include <tuple>
#include <typeinfo>
#include <math.h>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <random>
#include <complex>
#include <lua.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn_superres.hpp>
#include <opencv2/ximgproc.hpp>
#include <opencv2/xphoto/oilpainting.hpp>
#include <lunasvg.h>

using namespace cv;
using namespace dnn;
using namespace dnn_superres;
using namespace ximgproc;
using namespace xphoto;
using namespace Concurrency;
using namespace lunasvg;
#endif //PCH_H
