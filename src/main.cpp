#include "pch.h"

#pragma warning(disable:4244)
#pragma warning(disable:4819)
#pragma warning(disable:26451)

/********************************************************************************/
/*[prototype]*/

// main function
int medianblur(lua_State* L);
int boxblur(lua_State* L);
int gaussianblur(lua_State* L);
int bilateralfilter(lua_State* L);
int resizefilter(lua_State* L);
int resample(lua_State* L);
int sobelfilter(lua_State* L);
int sobelRGB(lua_State* L);
int laplacianfilter(lua_State* L);
int laplacianRGB(lua_State* L);
int cannyfilter(lua_State* L);
int cannyRGB(lua_State* L);
int morphologyfilter(lua_State* L);
int morphologyRGB(lua_State* L);
int ch_chg(lua_State* L);
int thres(lua_State* L);
int adpthres(lua_State* L);
int diffusion(lua_State* L);
int noise(lua_State* L);
int fig_blur(lua_State* L);
int moebius(lua_State* L);
int meanshift(lua_State* L);
int voronoi(lua_State* L);
int delaunay(lua_State* L);
int pencilskc(lua_State* L);
int stylize(lua_State* L);
int det_enhance(lua_State* L);
int edgePreserve(lua_State* L);
int oilpaint(lua_State* L);
int hist_eq(lua_State* L);
int color_map(lua_State* L);
int img_read(lua_State* L);
int img_call(lua_State* L);
int mov_read(lua_State* L);

// utility function
template<typename T>
T constrain(T v, T min, T max);
void GetLuaTable(lua_State* L, int stackIndex, std::vector<std::vector<std::any>>& vals, int key);
const char* toStringRestrict(lua_State* L, int idx) noexcept;
uintptr_t GetAddData(const char* modname, uintptr_t address);
std::tuple<void*, int, int> GetPixelData(lua_State* L, const char* mode);
void PutPixelData(lua_State* L, void* pixeldata);
void img_read_tmp(String path, int idx, Mat& out);

/********************************************************************************/
/*[c++ <=> lua]*/

static luaL_Reg functions[] = {
	{"medianblur", medianblur},
	{"boxblur", boxblur},
	{"gaussianblur", gaussianblur},
	{"bilateralfilter", bilateralfilter},
	{"resizefilter", resizefilter},
	{"resample", resample},
	{"sobelfilter", sobelfilter},
	{"sobelRGB", sobelRGB},
	{"laplacianfilter", laplacianfilter},
	{"laplacianRGB", laplacianRGB},
	{"cannyfilter", cannyfilter},
	{"cannyRGB", cannyRGB},
	{"morphologyfilter", morphologyfilter},
	{"morphologyRGB", morphologyRGB},
	{"ch_chg", ch_chg},
	{"thres",thres},
	{"adpthres",adpthres},
	{"diffusion", diffusion},
	{"noise", noise},
	{"fig_blur", fig_blur},
	{"moebius", moebius},
	{"meanshift", meanshift},
	{"voronoi", voronoi},
	{"delaunay", delaunay},
	{"pencilskc", pencilskc},
	{"stylize", stylize},
	{"det_enhance", det_enhance},
	{"edgePreserve", edgePreserve},
	{"oilpaint", oilpaint},
	{"hist_eq", hist_eq},
	{"color_map", color_map},
	{"img_read", img_read},
	{"img_call", img_call},
	{"mov_read", mov_read},
	{ nullptr, nullptr }
};

extern "C" {
	__declspec(dllexport) int luaopen_cvmods(lua_State* L)
	{
		luaL_register(L, "cvmods", functions);
		return 1;
	}
}

/********************************************************************************/
/*[callback function]*/

int medianblur(lua_State* L)
{
	int ksize = lua_tointeger(L, 1);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmp, out;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	cv::medianBlur(tmp, out, 2 * ksize + 1);

	PutPixelData(L, out.data);
	return 0;
}

int boxblur(lua_State* L)
{
	int boxW = lua_tointeger(L, 1);
	int boxH = lua_tointeger(L, 2);
	bool Normalize = static_cast<bool>(lua_toboolean(L, 3));
	int border = lua_tointeger(L, 4);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmp, out;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	std::vector<int> mode{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_REFLECT_101,
		BORDER_ISOLATED
	};

	cv::boxFilter(tmp, out, -1, Size(boxW, boxH), Point(-1, -1), Normalize, mode[border]);
	PutPixelData(L, out.data);
	return 0;
}

int gaussianblur(lua_State* L)
{
	int boxW = lua_tointeger(L, 1);
	int boxH = lua_tointeger(L, 2);
	double sigmaX = lua_tonumber(L, 3);
	double sigmaY = lua_tonumber(L, 4);
	int border = lua_tointeger(L, 5);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmp, out;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	std::vector<int> mode{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_REFLECT_101,
		BORDER_ISOLATED
	};

	int kx = constrain(2 * boxW - 1, 0, INT_MAX);
	int ky = constrain(2 * boxH - 1, 0, INT_MAX);
	cv::GaussianBlur(tmp, out, Size(kx, ky), sigmaX, sigmaY, mode[border]);

	PutPixelData(L, out.data);
	return 0;
}

int bilateralfilter(lua_State* L)
{
	int d = lua_tointeger(L, 1);
	double sigmacolor = lua_tonumber(L, 2);
	double sigmaspace = lua_tonumber(L, 3);
	int border = lua_tointeger(L, 4);
	bool alphasync = static_cast<bool>(lua_toboolean(L, 5));

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmp;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);


	Mat bgr(tmp.size(), CV_8UC3), alpha(tmp.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmp, 1, bgra.data(), 2, ch.data(), 4);

	std::vector<int> mode{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_WRAP,
		BORDER_REFLECT_101
	};

	Mat tmpB, tmpC;
	cv::bilateralFilter(bgra[0], tmpB, d, sigmacolor, sigmaspace, mode[border]);
	bgra[0] = tmpB.clone();
	if (alphasync)
	{
		cv::bilateralFilter(bgra[1], tmpC, d, sigmacolor, sigmaspace, mode[border]);
		bgra[1] = tmpC.clone();
	}


	Mat out(tmp.size(), img.type());
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);

	PutPixelData(L, out.data);

	return 0;
}

int resizefilter(lua_State* L)
{
	int new_w = lua_tointeger(L, 1);
	int new_h = lua_tointeger(L, 2);
	double coef_w = lua_tonumber(L, 3);
	double coef_h = lua_tonumber(L, 4);
	int resize_mode = lua_tointeger(L, 5);
	bool set_size = static_cast<bool>(lua_toboolean(L, 6));
	bool proto = static_cast<bool>(lua_toboolean(L, 7));

	auto dp = GetAddData("exedit.auf", 0x1b2b20);
	auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(dp);
	auto dt = &(exdata->obj_edit);
	auto wt = exdata->obj_w;
	auto ht = exdata->obj_h;
	auto ow = exdata->obj_line;
	auto oh = exdata->obj_max_h;

	Mat buf_bgra = cv::Mat::zeros(Size(wt, ht), CV_8UC4);
	if (proto)
	{
		auto [frame, width, height] = GetPixelData(L, "");
		Mat img(height, width, CV_8UC4, frame);
		if (img.empty())
		{
			std::cout << "Mat array is empty..." << std::endl;
			return 0;
		}
		img.copyTo(buf_bgra);
		wt = width;
		ht = height;
	}
	else
	{
		parallel_for(0, ht, 1, [&](int y)
			{
				auto pBGRA = buf_bgra.ptr<Vec4b>(y);

				for (auto x = 0; x < wt; ++x)
				{
					int idx = x + y * ow;

					int uy = static_cast<int>((*dt + idx)->y);
					int ucb = static_cast<int>((*dt + idx)->cb);
					int ucr = static_cast<int>((*dt + idx)->cr);
					int ua = static_cast<int>((*dt + idx)->a);

					pBGRA[x][0] = saturate_cast<uchar>((3 + ((uy * 16320) >> 16) + ((ucb * 28919) >> 16)) >> 2);
					pBGRA[x][1] = saturate_cast<uchar>((3 + ((uy * 16320) >> 16) + ((ucb * -5616) >> 16) + ((ucr * -11655) >> 16)) >> 2);
					pBGRA[x][2] = saturate_cast<uchar>((3 + ((uy * 16320) >> 16) + ((ucr * 22881) >> 16)) >> 2);
					pBGRA[x][3] = saturate_cast<uchar>((3 + ((ua * 16320) >> 16)) >> 2);
				}
			}
		);
	}

	std::vector<int> rsmode{
		INTER_NEAREST,
		INTER_LINEAR,
		INTER_CUBIC,
		INTER_AREA,
		INTER_LANCZOS4,
		INTER_LINEAR_EXACT,
		INTER_NEAREST_EXACT
	};

	Mat out;
	if (set_size)
	{
		new_w = constrain(new_w, 0, ow - 8);
		new_h = constrain(new_h, 0, oh);
		cv::resize(buf_bgra, out, cv::Size(new_w, new_h), 0.0, 0.0, rsmode[resize_mode]);
	}
	else
	{
		double cw_max = static_cast<double>(ow - 8) / wt;
		double ch_max = static_cast<double>(oh) / ht;
		coef_w = constrain(coef_w, 0.0, cw_max);
		coef_h = constrain(coef_h, 0.0, ch_max);
		cv::resize(buf_bgra, out, cv::Size(), coef_w, coef_h, rsmode[resize_mode]);
		new_w = out.cols;
		new_h = out.rows;
	}

	if ((ow - 8) < new_w || oh < new_h)
	{
		std::cout << "image is  too big..." << std::endl;
		return 0;
	}

	parallel_for(0, new_h, 1, [&](int y)
		{
			auto pOUT = out.ptr<Vec4b>(y);

			for (auto x = 0; x < new_w; ++x)
			{
				int idx = x + y * ow;
				int r = static_cast<int>(pOUT[x][2]);
				int g = static_cast<int>(pOUT[x][1]);
				int b = static_cast<int>(pOUT[x][0]);
				int a = static_cast<int>(pOUT[x][3]);

				int r_ = (r << 6) + 18;
				int g_ = (g << 6) + 18;
				int b_ = (b << 6) + 18;
				int a_ = (a << 6) + 1;

				(*dt + idx)->y = static_cast<int16_t>(((r_ * 4918) >> 16) + ((g_ * 9655) >> 16) + ((b_ * 1875) >> 16) - 3);
				(*dt + idx)->cb = static_cast<int16_t>(((r_ * -2775) >> 16) + ((g_ * -5449) >> 16) + ((b_ * 8224) >> 16) + 1);
				(*dt + idx)->cr = static_cast<int16_t>(((r_ * 8224) >> 16) + ((g_ * -6887) >> 16) + ((b_ * -1337) >> 16) + 1);
				(*dt + idx)->a = static_cast<int16_t>((a_ * 16448) >> 16);
			}
		}
	);
	exdata->obj_w = new_w;
	exdata->obj_h = new_h;

	return 0;
}

int resample(lua_State* L)
{
	double coef_w = lua_tonumber(L, 1);
	double coef_h = lua_tonumber(L, 2);
	int resize_exp = lua_tointeger(L, 3);
	int resize_red = lua_tointeger(L, 4);
	bool proto = static_cast<bool>(lua_toboolean(L, 5));

	auto dp = GetAddData("exedit.auf", 0x1b2b20);
	auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(dp);
	auto dt = &(exdata->obj_edit);
	auto wt = exdata->obj_w;
	auto ht = exdata->obj_h;
	auto ow = exdata->obj_line;
	auto oh = exdata->obj_max_h;

	Mat buf_bgra = cv::Mat::zeros(Size(wt, ht), CV_8UC4);
	if (proto)
	{
		auto [frame, width, height] = GetPixelData(L, "");
		Mat img(height, width, CV_8UC4, frame), tmp;
		if (img.empty())
		{
			std::cout << "Mat array is empty..." << std::endl;
			return 0;
		}
		img.copyTo(buf_bgra);
		wt = width;
		ht = height;
	}
	else
	{
		parallel_for(0, ht, 1, [&](int y)
			{
				auto pBGRA = buf_bgra.ptr<Vec4b>(y);

				for (auto x = 0; x < wt; ++x)
				{
					int idx = x + y * ow;

					int uy = static_cast<int>((*dt + idx)->y);
					int ucb = static_cast<int>((*dt + idx)->cb);
					int ucr = static_cast<int>((*dt + idx)->cr);
					int ua = static_cast<int>((*dt + idx)->a);

					pBGRA[x][0] = saturate_cast<uchar>((3 + ((uy * 16320) >> 16) + ((ucb * 28919) >> 16)) >> 2);
					pBGRA[x][1] = saturate_cast<uchar>((3 + ((uy * 16320) >> 16) + ((ucb * -5616) >> 16) + ((ucr * -11655) >> 16)) >> 2);
					pBGRA[x][2] = saturate_cast<uchar>((3 + ((uy * 16320) >> 16) + ((ucr * 22881) >> 16)) >> 2);
					pBGRA[x][3] = saturate_cast<uchar>((3 + ((ua * 16320) >> 16)) >> 2);
					
				}
			}
		);
	}

	Mat eximg, rdimg;
	std::vector<int> rsmode{
		INTER_NEAREST,
		INTER_LINEAR,
		INTER_CUBIC,
		INTER_AREA,
		INTER_LANCZOS4,
		INTER_LINEAR_EXACT,
		INTER_NEAREST_EXACT
	};

	double cw_max = static_cast<double>(ow - 8) / wt;
	double ch_max = static_cast<double>(oh) / ht;
	coef_w = constrain(0.01 * coef_w, 0.0, cw_max);
	coef_h = constrain(0.01 * coef_h, 0.0, ch_max);
	cv::resize(buf_bgra, rdimg, Size(), coef_w, coef_h, rsmode[resize_red]);

	double cx = static_cast<double>(wt) / rdimg.cols;
	double cy = static_cast<double>(ht) / rdimg.rows;
	cv::resize(rdimg, eximg, Size(), cx, cy, rsmode[resize_exp]);

	int new_w = eximg.cols;
	int new_h = eximg.rows;

	if ((ow - 8) < new_w || oh < new_h)
	{
		std::cout << "image is  too big..." << std::endl;
		return 0;
	}

	parallel_for(0, new_h, 1, [&](int y)
		{
			auto pOUT = eximg.ptr<Vec4b>(y);

			for (auto x = 0; x < new_w; ++x)
			{
				int idx = x + y * ow;
				int r = static_cast<int>(pOUT[x][2]);
				int g = static_cast<int>(pOUT[x][1]);
				int b = static_cast<int>(pOUT[x][0]);
				int a = static_cast<int>(pOUT[x][3]);

				int r_ = (r << 6) + 18;
				int g_ = (g << 6) + 18;
				int b_ = (b << 6) + 18;
				int a_ = (a << 6) + 1;

				(*dt + idx)->y = static_cast<int16_t>(((r_ * 4918) >> 16) + ((g_ * 9655) >> 16) + ((b_ * 1875) >> 16) - 3);
				(*dt + idx)->cb = static_cast<int16_t>(((r_ * -2775) >> 16) + ((g_ * -5449) >> 16) + ((b_ * 8224) >> 16) + 1);
				(*dt + idx)->cr = static_cast<int16_t>(((r_ * 8224) >> 16) + ((g_ * -6887) >> 16) + ((b_ * -1337) >> 16) + 1);
				(*dt + idx)->a = static_cast<int16_t>((a_ * 16448) >> 16);
			}
		}
	);
	exdata->obj_w = new_w;
	exdata->obj_h = new_h;

	return 0;
}

int sobelfilter(lua_State* L)
{
	int ksize = lua_tointeger(L, 1);
	bool balp = static_cast<bool>(lua_toboolean(L, 2));
	int dx = lua_tointeger(L, 3);
	int dy = lua_tointeger(L, 4);
	int border = lua_tointeger(L, 5);
	double scale = lua_tonumber(L, 6);
	double delta = lua_tonumber(L, 7);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	Mat tmpB, tmpC, tmpD, tmpE;
	cv::cvtColor(bgr, tmpB, COLOR_BGR2GRAY);
	tmpB.convertTo(tmpC, CV_32F, 1.0 / 255, 0.0);

	std::vector<int> mode
	{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_REFLECT_101,
		BORDER_ISOLATED
	};

	ksize = constrain(2 * ksize - 1, 0, INT_MAX);
	cv::Sobel(tmpC, tmpD, -1, dx, dy, ksize, scale, delta, mode[border]);
	cv::convertScaleAbs(tmpD, tmpE, 255.0, 0.0);

	if (balp)
		cv::threshold(tmpE, bgra[1], 0.1, 255, THRESH_BINARY);

	Mat out;
	std::vector<Mat> ch_out{ tmpE, tmpE, tmpE, bgra[1] };
	cv::merge(ch_out, out);
	PutPixelData(L, out.data);
	return 0;
}

int sobelRGB(lua_State* L)
{
	int ksize = lua_tointeger(L, 1);
	bool balp = static_cast<bool>(lua_toboolean(L, 2));
	int dx = lua_tointeger(L, 3);
	int dy = lua_tointeger(L, 4);
	int border = lua_tointeger(L, 5);
	double scale = lua_tonumber(L, 6);
	double delta = lua_tonumber(L, 7);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA, tmpB;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);
	tmpA.convertTo(tmpB, CV_32FC4, 1.0 / 255, 0.0);

	std::vector<Mat> ch_col;
	cv::split(tmpB, ch_col);

	std::vector<int> mode
	{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_REFLECT_101,
		BORDER_ISOLATED
	};

	ksize = constrain(2 * ksize - 1, 0, INT_MAX);
	Mat tmpC = Mat::zeros(tmpA.size(), CV_32F);
	Mat tmpD, tmpF;
	for (auto i = 0; i < 3; ++i)
	{
		Mat tmpE;
		cv::Sobel(ch_col[i], tmpE, -1, dx, dy, ksize, scale, delta, mode[border]);
		cv::add(tmpC, tmpE, tmpC);
	}
	cv::convertScaleAbs(tmpC, tmpD, 255.0, 0.0);
		
	if (balp)
		cv::threshold(tmpD, tmpF, 0.1, 255, THRESH_BINARY);
	else
		cv::convertScaleAbs(ch_col[3], tmpF, 255.0, 0.0);

	Mat out;
	std::vector<Mat> ch_out{ tmpD, tmpD, tmpD, tmpF };
	cv::merge(ch_out, out);
	PutPixelData(L, out.data);
	return 0;
}

int laplacianfilter(lua_State* L)
{
	int ksize = lua_tointeger(L, 1);
	bool balp = static_cast<bool>(lua_toboolean(L, 2));
	int border = lua_tointeger(L, 3);
	double scale = lua_tonumber(L, 4);
	double delta = lua_tonumber(L, 5);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	Mat tmpB, tmpC, tmpD, tmpE;
	cv::cvtColor(tmpA, tmpB, COLOR_BGRA2GRAY);
	tmpB.convertTo(tmpC, CV_32F, 1.0 / 255.0, 0.0);

	std::vector<int> mode{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_REFLECT_101,
		BORDER_ISOLATED
	};

	ksize = constrain(2 * ksize - 1, 0, INT_MAX);
	cv::Laplacian(tmpC, tmpD, -1, ksize, scale, delta, mode[border]);
	cv::convertScaleAbs(tmpD, tmpE, 255.0, 0.0);

	if (balp)
		cv::threshold(tmpE, bgra[1], 0.1, 255, THRESH_BINARY);

	Mat out;
	std::vector<Mat> ch_out{ tmpE, tmpE, tmpE, bgra[1] };
	cv::merge(ch_out, out);
	PutPixelData(L, out.data);
	return 0;
}

int laplacianRGB(lua_State* L)
{
	int ksize = lua_tointeger(L, 1);
	bool balp = static_cast<bool>(lua_toboolean(L, 2));
	int border = lua_tointeger(L, 3);
	double scale = lua_tonumber(L, 4);
	double delta = lua_tonumber(L, 5);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA, tmpB;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);
	tmpA.convertTo(tmpB, CV_32FC4, 1.0 / 255, 0.0);

	std::vector<Mat> ch_col;
	cv::split(tmpB, ch_col);

	std::vector<int> mode{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_REFLECT_101,
		BORDER_ISOLATED
	};
	ksize = constrain(2 * ksize - 1, 0, INT_MAX);

	Mat tmpC = Mat::zeros(tmpA.size(), CV_32F);
	Mat tmpD, tmpF;
	for (auto i = 0; i < 3; ++i)
	{
		Mat tmpE;
		cv::Laplacian(ch_col[i], tmpE, -1, ksize, scale, delta, mode[border]);
		cv::add(tmpC, tmpE, tmpC);
	}
	cv::convertScaleAbs(tmpC, tmpD, 255.0, 0.0);

	if (balp)
		cv::threshold(tmpD, tmpF, 0.1, 255, THRESH_BINARY);
	else
		cv::convertScaleAbs(ch_col[3], tmpF, 255.0, 0.0);

	Mat out;
	std::vector<Mat> ch_out{ tmpD, tmpD, tmpD, tmpF };
	cv::merge(ch_out, out);
	PutPixelData(L, out.data);
	return 0;
}

int cannyfilter(lua_State* L)
{
	double thres1 = lua_tonumber(L, 1);
	double thres2 = lua_tonumber(L, 2);
	int asize = lua_tointeger(L, 3);
	bool gradient = static_cast<bool>(lua_toboolean(L, 4));
	bool balp = static_cast<bool>(lua_toboolean(L, 5));

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	Mat tmpB, tmpC;
	cv::cvtColor(bgr, tmpB, COLOR_BGRA2GRAY);
	cv::Canny(tmpB, tmpC, thres1, thres2, 2 * asize - 1, gradient);

	if (balp)
		cv::threshold(tmpC, bgra[1], 0.1, 255, THRESH_BINARY);

	Mat out;
	std::vector<Mat> ch_out{ tmpC, tmpC, tmpC, bgra[1] };
	cv::merge(ch_out, out);
	PutPixelData(L, out.data);
	return 0;
}

int cannyRGB(lua_State* L)
{
	double thres1 = lua_tonumber(L, 1);
	double thres2 = lua_tonumber(L, 2);
	int asize = lua_tointeger(L, 3);
	bool gradient = static_cast<bool>(lua_toboolean(L, 4));
	bool balp = static_cast<bool>(lua_toboolean(L, 5));

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	std::vector<Mat> ch_col;
	cv::split(tmpA, ch_col);

	Mat tmpB = Mat::zeros(tmpA.size(), CV_8U);
	for (auto i = 0; i < 3; ++i)
	{
		Mat tmpC;
		cv::Canny(ch_col[i], tmpC, thres1, thres2, 2 * asize - 1, gradient);
		cv::add(tmpB, tmpC, tmpB);
	}

	if (balp)
		cv::threshold(tmpB, ch_col[3], 0.1, 255, THRESH_BINARY);

	Mat out;
	std::vector<Mat> ch_out{ tmpB, tmpB, tmpB, ch_col[3] };
	cv::merge(ch_out, out);
	PutPixelData(L, out.data);
	return 0;
}

int morphologyfilter(lua_State* L)
{
	int ksize = lua_tointeger(L, 1);
	int ktype = lua_tointeger(L, 2);
	int mode = lua_tointeger(L, 3);
	int iterations = lua_tointeger(L, 4);
	int border = lua_tointeger(L, 5);
	bool alphasync = static_cast<bool>(lua_toboolean(L, 6));

	luaL_openlibs(L);
	lua_getglobal(L, "anc");
	int stackidx = lua_gettop(L);
	lua_pushnil(L);
	std::array<int, 2> pt{ 0, 0 };
	int j = 0;
	while (lua_next(L, stackidx) != 0)
	{
		if (lua_isnumber(L, -1))
		{
			pt[j] = lua_tointeger(L, -1);
			++j;
		}
		lua_pop(L, 1);
	}

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	std::vector<int> kshape{
		MORPH_RECT,
		MORPH_CROSS,
		MORPH_ELLIPSE
	};

	std::vector<int> opmode{
		MORPH_ERODE,
		MORPH_DILATE,
		MORPH_OPEN,
		MORPH_CLOSE,
		MORPH_GRADIENT,
		MORPH_TOPHAT,
		MORPH_BLACKHAT,
		MORPH_HITMISS
	};

	std::vector<int> bmode{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_REFLECT_101,
		BORDER_ISOLATED
	};

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	img.copyTo(tmpA);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	Mat tmpB = Mat::zeros(tmpA.size(), CV_8UC3);
	Mat tmpC, tmpD;
	
	bgra[0].copyTo(tmpB, bgra[1]);
	cv::cvtColor(tmpB, tmpB, COLOR_BGR2GRAY);
	ksize = constrain(2 * ksize - 1, 1, INT_MAX);
	Point anchor(pt[0], pt[1]);
	Mat kernel = getStructuringElement(kshape[ktype], Size(ksize, ksize), anchor);
	cv::morphologyEx(tmpB, tmpC, opmode[mode], kernel, anchor, iterations, bmode[border]);

	if (alphasync)
		cv::morphologyEx(bgra[1], tmpD, opmode[mode], kernel, anchor, iterations, bmode[border]);
	else
		tmpD = bgra[1].clone();

	Mat out;
	std::vector<Mat> ch_out{ tmpC, tmpC, tmpC, tmpD };
	cv::merge(ch_out, out);
	PutPixelData(L, out.data);

	return 0;
}

int morphologyRGB(lua_State* L)
{
	int ksizeB = lua_tointeger(L, 1);
	int ksizeG = lua_tointeger(L, 2);
	int ksizeR = lua_tointeger(L, 3);
	int ktype = lua_tointeger(L, 4);
	int mode = lua_tointeger(L, 5);
	int iterations = lua_tointeger(L, 6);
	int border = lua_tointeger(L, 7);
	bool alphasync = static_cast<bool>(lua_toboolean(L, 8));

	luaL_openlibs(L);
	lua_getglobal(L, "anc");
	int stackidx = lua_gettop(L);
	lua_pushnil(L);
	std::array<int, 2> pt{ 0, 0 };
	int j = 0;
	while (lua_next(L, stackidx) != 0)
	{
		if (lua_isnumber(L, -1))
		{
			pt[j] = lua_tointeger(L, -1);
			++j;
		}
		lua_pop(L, 1);
	}

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat tmpB = Mat::zeros(tmpA.size(), CV_8UC3);
	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	img.copyTo(tmpA);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);
	bgra[0].copyTo(tmpB, bgra[1]);

	std::vector<Mat> col_ch;
	cv::split(tmpB, col_ch);

	std::vector<int> kshape{
		MORPH_RECT,
		MORPH_CROSS,
		MORPH_ELLIPSE
	};

	std::vector<int> opmode{
		MORPH_ERODE,
		MORPH_DILATE,
		MORPH_OPEN,
		MORPH_CLOSE,
		MORPH_GRADIENT,
		MORPH_TOPHAT,
		MORPH_BLACKHAT,
		MORPH_HITMISS
	};

	std::vector<int> bmode{
		BORDER_CONSTANT,
		BORDER_REPLICATE,
		BORDER_REFLECT,
		BORDER_REFLECT_101,
		BORDER_ISOLATED
	};

	std::vector<int> ksize{ ksizeB, ksizeG, ksizeR };
	Point anchor(pt[0], pt[1]);
	std::vector<Mat> kernel;
	for (auto i = 0; i < 3; ++i)
	{
		Mat tmpB = getStructuringElement(kshape[ktype], Size(2 * ksize[i] - 1, 2 * ksize[i] - 1), anchor);
		kernel.emplace_back(tmpB);
		col_ch.emplace_back(bgra[1]);
	}

	Mat tmpC = Mat::zeros(tmpA.size(), CV_8U);
	std::vector<Mat> tmpD(6, tmpC);
	iterations = constrain(iterations, 1, INT_MAX);
	parallel_for(0, 6, 1, [&](int j)
		{
			Mat tmpE;
			cv::morphologyEx(col_ch[j], tmpE, opmode[mode], kernel[j % 3], anchor, iterations, bmode[border]);
			tmpD[j] = tmpE.clone();
		}
	);
	if (alphasync)
	{
		for (auto i = 0; i < 3; ++i)
		{
			cv::add(tmpC, tmpD[3 + i], tmpC);
		}
	}
	else
	{
		col_ch[3].copyTo(tmpC);
	}

	Mat out;
	std::vector ch_out{ tmpD[0], tmpD[1], tmpD[2], tmpC};
	cv::merge(ch_out, out);

	PutPixelData(L, out.data);
	return 0;
}

int ch_chg(lua_State* L)
{
	int ch_red = lua_tointeger(L, 1);
	int ch_green = lua_tointeger(L, 2);
	int ch_blue = lua_tointeger(L, 3);
	int ch_alpha = lua_tointeger(L, 4);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmp;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	Mat out(tmp.size(), tmp.type());
	std::vector<int> ch{ ch_blue,0, ch_green,1, ch_red,2, ch_alpha,3 };
	cv::mixChannels(&tmp, 1, &out, 1, ch.data(), 4);

	PutPixelData(L, out.data);
	return 0;
}

int thres(lua_State* L)
{
	double thres = lua_tonumber(L, 1);
	double maxv = lua_tonumber(L, 2);
	int tmode = lua_tointeger(L, 3);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);
	
	std::vector<int> mode{
		THRESH_BINARY,
		THRESH_BINARY_INV,
		THRESH_TRUNC,
		THRESH_TOZERO,
		THRESH_TOZERO_INV,
		THRESH_OTSU,
		THRESH_TRIANGLE
	};

	Mat tmpB, tmpC;
	cvtColor(bgr, tmpB, COLOR_BGR2GRAY);
	cv::threshold(tmpB, tmpC, thres, maxv, mode[tmode]);

	Mat out;
	std::vector<Mat> ch_out{ tmpC, tmpC, tmpC, bgra[1] };
	cv::merge(ch_out, out);

	PutPixelData(L, out.data);
	return 0;
}

int adpthres(lua_State* L)
{
	int adpthres = lua_tointeger(L, 1);
	double maxv = lua_tonumber(L, 2);
	int bsize = lua_tointeger(L, 3);
	int tmode = lua_tointeger(L, 4);
	double valC = lua_tonumber(L, 5);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA, tmpB;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);
	cv::cvtColor(bgra[0], tmpB, COLOR_BGR2GRAY);

	std::vector<int> mode{
		THRESH_BINARY_INV,
		THRESH_BINARY
	};

	std::vector<int> adp_mode{
		ADAPTIVE_THRESH_MEAN_C,
		ADAPTIVE_THRESH_GAUSSIAN_C
	};

	Mat tmpC;
	tmode = constrain(tmode, 0, 1);
	adpthres = constrain(adpthres, 0, 1);
	cv::adaptiveThreshold(tmpB, tmpC, maxv, adp_mode[adpthres], mode[tmode], 2 * bsize + 1, valC);

	Mat out;
	std::vector<Mat> ch_out{ tmpC, tmpC, tmpC, bgra[1] };
	cv::merge(ch_out, out);
	PutPixelData(L, out.data);
	return 0;
}

int diffusion(lua_State* L)
{
	int iter = lua_tointeger(L, 1);
	float decay_factor = static_cast<float>(lua_tonumber(L, 2));
	float decay_offset = static_cast<float>(lua_tonumber(L, 3));
	float gamma = static_cast<float>(lua_tonumber(L, 4));
	int sigma = constrain<int>(lua_tointeger(L, 5), 0, INT_MAX);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA, tmpB;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);
	tmpA.convertTo(tmpB, CV_32FC4, 1.0 / 255.0);

	Mat tmpC = cv::Mat::zeros(tmpB.size(), CV_32FC3);
	parallel_for(0, height, 1, [&](int y)
		{
			auto pIN = tmpB.ptr<Vec4f>(y);
			auto pPOW = tmpC.ptr<Vec3f>(y);

			for (auto x = 0; x < width; ++x)
			{
				pPOW[x][0] = std::pow(pIN[x][0], gamma);
				pPOW[x][1] = std::pow(pIN[x][1], gamma);
				pPOW[x][2] = std::pow(pIN[x][2], gamma);
			}
		}
	);

	Mat tmpE = tmpC.clone();
	for (int i = 0; i < iter; ++i)
	{
		float gain = std::pow(decay_factor, -((float)i + decay_offset));
		sigma *= sigma;
		Mat tmpD;
		cv::GaussianBlur(tmpC, tmpD, Size(), (double)sigma);

		parallel_for(0, height, 1, [&tmpD, &tmpE, gain, width](int y)
			{
				auto pIN1 = tmpD.ptr<Vec3f>(y);
				auto pIN2 = tmpE.ptr<Vec3f>(y);

				for (auto x = 0; x < width; ++x)
				{
					pIN2[x][0] += (pIN1[x][0] * gain);
					pIN2[x][1] += (pIN1[x][1] * gain);
					pIN2[x][2] += (pIN1[x][2] * gain);
				}
			}
		);
		tmpD.release();
	}

	float gm_inv = 1.0 / gamma;
	Mat tmpF = cv::Mat::zeros(tmpB.size(), tmpB.type());
	parallel_for(0, height, 1, [&tmpE, &tmpF, &tmpB, gm_inv, width](int y)
		{
			auto pIN3 = tmpE.ptr<Vec3f>(y);
			auto pIN4 = tmpB.ptr<Vec4f>(y);
			auto pOUT = tmpF.ptr<Vec4f>(y);

			for (auto x = 0; x < width; ++x)
			{
				pOUT[x][0] = constrain(std::pow(pIN3[x][0], gm_inv), 0.f, 1.f);
				pOUT[x][1] = constrain(std::pow(pIN3[x][1], gm_inv), 0.f, 1.f);
				pOUT[x][2] = constrain(std::pow(pIN3[x][2], gm_inv), 0.f, 1.f);
				pOUT[x][3] = pIN4[x][3];
			}
		}
	);

	Mat out;
	convertScaleAbs(tmpF, out, 255.0, 0.0);
	PutPixelData(L, out.data);
	return 0;
}

int noise(lua_State* L)
{
	float alp = static_cast<float>(lua_tonumber(L, 1));
	float valK = static_cast<float>(lua_tonumber(L, 2));
	int iter = lua_tointeger(L, 3);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	Mat tmpB, out(tmpA.size(), tmpA.type());
	anisotropicDiffusion(bgra[0], tmpB, alp, valK, iter);
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int fig_blur(lua_State* L)
{
	double size = lua_tonumber(L, 1);
	double cbrt = lua_tonumber(L, 2);
	auto fpath = toStringRestrict(L, 3);
	
	try
	{
		Mat k1 = cv::imread(fpath, IMREAD_GRAYSCALE);

		// size : sy = col : rows
		int sx = static_cast<int>(size);
		int sy = static_cast<int>(double(sx) * k1.rows / k1.cols);
		Mat tmpA;
		cv::resize(k1, tmpA, Size(sx, sy), 0.0, 0.0, INTER_LANCZOS4);

		Mat tmpB, tmpC, kernel;
		Mat tmpD = Mat::ones(tmpA.size(), CV_32F);
		tmpA.convertTo(tmpB, CV_32F, 1.0, 0.0);
		cv::Scalar sums = cv::sum(tmpB);
		cv::divide(tmpB, tmpD * sums[0], tmpC);
		cv::multiply(tmpC, tmpD*cbrt, kernel);

		auto [frame, width, height] = GetPixelData(L, "");
		Mat img(height, width, CV_8UC4, frame), in0;
		if (img.empty())
		{
			std::cout << "Mat array is empty..." << std::endl;
			return 0;
		}
		img.copyTo(in0);

		Mat bgr(img.size(), CV_8UC3), alpha(img.size(), CV_8U);
		std::vector<Mat> bgra{ bgr, alpha };
		std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
		cv::mixChannels(&in0, 1, bgra.data(), 2, ch.data(), 4);
		
		Mat in1, in2, out1, out2;
		bgra[0].convertTo(in1, CV_32F);
		bgra[1].convertTo(in2, CV_32F);
		cv::filter2D(in1, out1, -1, kernel);
		cv::filter2D(in2, out2, -1, kernel);

		Mat out3(img.size(), CV_8UC4);
		cv::convertScaleAbs(out1, bgra[0], 1.0, 0.0);
		cv::convertScaleAbs(out2, bgra[1], 1.0, 0.0);
		cv::mixChannels(bgra.data(), 2, &out3, 1, ch.data(), 4);
		PutPixelData(L, out3.data);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

void GetPtElement(lua_State* L, int stackIndex, std::vector<std::vector<double>>& tb, int key)
{
	std::vector<double> elements;
	int stackHead = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, stackHead) != 0)
	{
		if (lua_istable(L, -1))
		{
			tb.emplace_back(elements);
			GetPtElement(L, lua_gettop(L), tb, key+1);
		}
		else
		{
			if (lua_isnumber(L, -1))
			{
				double ele = lua_tonumber(L, -1);
				tb[key].emplace_back(ele);
			}
		}
		lua_pop(L, 1);
	}
}

int moebius(lua_State* L)
{
	try
	{
		bool normalize = static_cast<bool>(lua_toboolean(L, 1));
		int iterX = lua_tointeger(L, 2);
		int iterY = lua_tointeger(L, 3);

		std::vector<std::vector<double>> coef(4, { 0, 0 });
		std::vector<std::vector<double>> cf;
		luaL_openlibs(L);
		lua_getglobal(L, "coef");

		int cfidx = 0;
		if (lua_checkstack(L, 100) == 0)
			return 0;

		std::vector<double> init;
		cf.emplace_back(init);
		GetPtElement(L, lua_gettop(L), cf, 0);
		
		int range = cf[0][0] + 1;
		std::copy(cf.begin() + 1, cf.begin() + range, coef.begin());

		Mat buf_bgra;
		auto [frame, width, height] = GetPixelData(L, "");
		Mat img(height, width, CV_8UC4, frame);
		if (img.empty())
		{
			std::cout << "Mat array is empty..." << std::endl;
			return 0;
		}
		img.copyTo(buf_bgra);

		// デバッグ
		//std::vector<std::complex<double>> c4(4, std::complex<double>(0.0, 0.0));
		//for (auto i = 0; i < 4; ++i)
		//{
		//	c4[i] = std::complex<double>(coef[i][0], coef[i][1]);
		//	std::cout << c4[i] << std::endl;
		//}

		std::complex<double> ca(coef[0][0], coef[0][1]);
		std::complex<double> cb(coef[1][0], coef[1][1]);
		std::complex<double> cc(coef[2][0], coef[2][1]);
		std::complex<double> cd(coef[3][0], coef[3][1]);

		if (ca * cd == cb * cc)
			return 0;

		iterX = constrain(iterX, 1, INT_MAX);
		iterY = constrain(iterY, 1, INT_MAX);
		int input_w = iterX * width;
		int input_h = iterY * height;
		int csize = input_w * input_h;
		std::vector<double> coodX(csize, 0);
		std::vector<double> coodY(csize, 0);
		parallel_for(0, csize, 1, [&](int i)
			{
				int x = i % input_w;
				int y = i / input_w;

				double nx = double(x);
				double ny = double(y);
				if (normalize)
				{
					nx -= double(input_w) / 2;
					ny -= double(input_h) / 2;
				}

				std::complex<double> pos(double(nx) / input_w, double(ny) / input_h);
				std::complex<double> fz = (ca * pos + cb) / (cc * pos + cd);

				coodX[i] = fz.real();
				coodY[i] = fz.imag();
			}
		);

		double xmax = *max_element(coodX.begin(), coodX.end());
		double xmin = *min_element(coodX.begin(), coodX.end());
		double ymax = *max_element(coodY.begin(), coodY.end());
		double ymin = *min_element(coodY.begin(), coodY.end());
		double xrange = (xmax == xmin) ? 1.0 : (xmax - xmin);
		double yrange = (ymax == ymin) ? 1.0 : (ymax - ymin);

		Mat tmp(Size(input_w, input_h), buf_bgra.type());
		parallel_for(0, input_h, 1, [&](int y)
			{
				auto pOUT = tmp.ptr<Vec4b>(y);
				for (auto x = 0; x < input_w; ++x)
				{
					int cdi = x + y * input_w ;
					int new_x = int((input_w - 1) * (coodX[cdi] - xmin) / xrange);
					int new_y = int((input_h - 1) * (coodY[cdi] - ymin) / yrange);

					new_x = new_x % width;
					new_y = new_y % height;
					
					auto pIN = buf_bgra.ptr<Vec4b>(new_y);
					pOUT[x] = pIN[new_x];
				}
			}
		);

		Mat out;
		cv::resize(tmp, out, Size(width, height), 0, 0, INTER_LANCZOS4);
		PutPixelData(L, out.data);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

int meanshift(lua_State* L)
{
	double sp = lua_tonumber(L, 1);
	double sr = lua_tonumber(L, 2);
	int maxl = lua_tointeger(L, 3);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	Mat tmpB, out(tmpA.size(), tmpA.type());
	cv::pyrMeanShiftFiltering(bgra[0], tmpB, sp, sr, maxl);
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int voronoi(lua_State* L)
{
	int pt = lua_tointeger(L, 1);
	int seed = lua_tointeger(L, 2);
	bool lindrw = static_cast<bool>(lua_toboolean(L, 3));
	int red = lua_tointeger(L, 4);
	int green = lua_tointeger(L, 5);
	int blue = lua_tointeger(L, 6);
	int linth = lua_tointeger(L, 7);
	int lintp = lua_tointeger(L, 8);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Subdiv2D subdiv;
	subdiv.initDelaunay(cv::Rect(0, 0, width, height));
	std::vector<Point2f> points(pt, Point2f(0.f, 0.f)), facecenter;
	std::seed_seq sq{ seed };
	std::mt19937 rn(sq);

	for (auto i = 0; i < pt; ++i)
	{
		uint rnd = rn();
		float px = static_cast<float>(rnd % width);
		float py = static_cast<float>(rnd % height);
		points[i] = Point2f(px, py);
	}

	points.emplace_back(Point2f(0, 0));
	points.emplace_back(Point2f(width - 1, 0));
	points.emplace_back(Point2f(0, height - 1));
	points.emplace_back(Point2f(width - 1, height - 1));
	subdiv.insert(points);

	std::vector<int> idx;
	std::vector<std::vector<Point2f>> facetlist;

	subdiv.getVoronoiFacetList(idx, facetlist, facecenter);
	Mat vn = Mat::zeros(tmpA.size(), CV_8UC3);
	std::vector<int> ltype{ LINE_4, LINE_8, LINE_AA };
	int linType = ltype[lintp];

	parallel_for_each(facetlist.begin(), facetlist.end(), [&](std::vector<Point2f> ftl)
		{
			std::vector<Point> vertices(ftl.size(), Point());
			auto polyColor = Vec3f(0.f, 0.f, 0.f);
			int polyColorNum = 0;
			int psize = ftl.size();

			for (int i = 0; i < psize; ++i)
			{
				Point vertex = ftl[i];
				vertices[i] = vertex;
				int vx = vertex.x;
				int vy = vertex.y;

				if (0 <= vx && vx < bgr.cols && 0 <= vy && vy < bgr.rows)
				{
					auto pIMG = bgra[0].ptr<Vec3b>(vy);
					polyColor += pIMG[vx];
					polyColorNum++;
				}
			}
			polyColor /= float(polyColorNum);
			cv::fillConvexPoly(vn, vertices.data(), psize, cv::Scalar(polyColor), linType);
			vertices.clear();
		}
	);

	if (lindrw)
	{
		cv::Scalar lincol(blue, green, red);
		parallel_for_each(facetlist.begin(), facetlist.end(), [&](std::vector<Point2f> ftl)
			{
				Point2f p1 = ftl[0];
				int psize = ftl.size();
				for (int i = 0; i < psize; ++i)
				{
					Point2f p2 = ftl[i];
					cv::line(vn, p1, p2, lincol, linth, linType);
					p1 = p2;
				}
			}
		);
	}

	Mat out(tmpA.size(), tmpA.type());
	bgra[0] = vn.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int delaunay(lua_State* L)
{
	int pt = lua_tointeger(L, 1);
	int seed = lua_tointeger(L, 2);
	bool lindrw = static_cast<bool>(lua_toboolean(L, 3));
	int red = lua_tointeger(L, 4);
	int green = lua_tointeger(L, 5);
	int blue = lua_tointeger(L, 6);
	int linth = lua_tointeger(L, 7);
	int lintp = lua_tointeger(L, 8);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Subdiv2D subdiv;
	subdiv.initDelaunay(cv::Rect(0, 0, width, height));
	std::vector<Point2f> points(pt, Point2f(0.f, 0.f));
	std::seed_seq sq{ seed };
	std::mt19937 rn(sq);
	for (auto i = 0; i < pt; ++i)
	{
		uint rnd = rn();
		float px = static_cast<float>(rnd % width);
		float py = static_cast<float>(rnd % height);
		points[i] = Point2f(px, py);
	}

	points.emplace_back(Point2f(0, 0));
	points.emplace_back(Point2f(width - 1, 0));
	points.emplace_back(Point2f(0, height - 1));
	points.emplace_back(Point2f(width - 1, height - 1));
	subdiv.insert(points);

	std::vector<Vec4f> edgelist;
	std::vector<Vec6f> trilist;

	subdiv.getTriangleList(trilist);
	subdiv.getEdgeList(edgelist);
	Mat dn = Mat::zeros(tmpA.size(), CV_8UC3);
	std::vector<int> ltype{ LINE_4, LINE_8, LINE_AA };
	int linType = ltype[lintp];

	parallel_for_each(trilist.begin(), trilist.end(), [&](Vec6f tri)
		{
			std::vector<Point> vertices(3, Point());
			auto polyColor = Vec3f(0.f, 0.f, 0.f);

			vertices[0] = Point(tri[0], tri[1]);
			vertices[1] = Point(tri[2], tri[3]);
			vertices[2] = Point(tri[4], tri[5]);

			for (int i = 0; i < 3; ++i)
			{
				auto pIMG = bgra[0].ptr<Vec3b>(vertices[i].y);
				polyColor += pIMG[vertices[i].x];
			}
			polyColor /= 3.f;
			cv::fillConvexPoly(dn, vertices.data(), 3, cv::Scalar(polyColor), linType);
			vertices.clear();
		}
	);

	if (lindrw)
	{
		cv::Scalar lincol(blue, green, red);
		parallel_for_each(edgelist.begin(), edgelist.end(), [&dn, &lincol, linth, linType](Vec4f edges)
			{
				cv::line(dn, Point(edges[0], edges[1]), Point(edges[2], edges[3]), lincol, linth, linType);
			}
		);
	}

	Mat out(tmpA.size(), tmpA.type());
	bgra[0] = dn.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int pencilskc(lua_State* L)
{
	float sgmS = static_cast<float>(lua_tonumber(L, 1));
	float sgmR = static_cast<float>(lua_tonumber(L, 2));
	float shade = static_cast<float>(lua_tonumber(L, 3));
	bool color = static_cast<bool>(lua_toboolean(L, 4));

	auto [frame, width, height] = GetPixelData(L, "");
	Mat imgA(height, width, CV_8UC4, frame), tmpA;
	if (imgA.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	imgA.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	sgmS = constrain(sgmS, 0.f, 200.f);
	sgmR = constrain(sgmR, 0.f, 1.f);
	shade = constrain(shade, 0.f, 0.1f);

	Mat tmpB, tmpC, tmpD;
	pencilSketch(bgr, tmpB, tmpC, sgmS, sgmR, shade);
	if (color)
		tmpD = tmpC.clone();
	else
		cvtColor(tmpB, tmpD, COLOR_GRAY2BGR);

	Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpD.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int stylize(lua_State* L)
{
	float sgmS = static_cast<float>(lua_tonumber(L, 1));
	float sgmR = static_cast<float>(lua_tonumber(L, 2));

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	sgmS = constrain(sgmS, 0.f, 200.f);
	sgmR = constrain(sgmR, 0.f, 1.f);
	Mat tmpB;
	stylization(bgr, tmpB, sgmS, sgmR);

	Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int det_enhance(lua_State* L)
{
	float sgmS = static_cast<float>(lua_tonumber(L, 1));
	float sgmR = static_cast<float>(lua_tonumber(L, 2));

	auto [frame, width, height] = GetPixelData(L, "");
	Mat imgA(height, width, CV_8UC4, frame), tmpA;
	if (imgA.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	imgA.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	sgmS = constrain(sgmS, 0.f, 200.f);
	sgmR = constrain(sgmR, 0.f, 1.f);

	Mat tmpB;
	detailEnhance(bgr, tmpB, sgmS, sgmR);

	Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int edgePreserve(lua_State* L)
{
	float sgmS = static_cast<float>(lua_tonumber(L, 1));
	float sgmR = static_cast<float>(lua_tonumber(L, 2));
	int mode = lua_tointeger(L, 3);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	std::vector<int> flags{
		RECURS_FILTER,
		NORMCONV_FILTER
	};

	sgmS = constrain(sgmS, 0.f, 200.f);
	sgmR = constrain(sgmR, 0.f, 1.f);
	Mat tmpB;
	edgePreservingFilter(bgr, tmpB, flags[mode], sgmS, sgmR);

	Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int oilpaint(lua_State* L)
{
	int size = lua_tointeger(L, 1);
	int ratio = lua_tointeger(L, 2);

	auto [frame, width, height] = GetPixelData(L, "");
	Mat imgA(height, width, CV_8UC4, frame), tmpA;
	if (imgA.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	imgA.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	size = constrain(size, 2, INT_MAX);
	Mat tmpB;
	oilPainting(bgr, tmpB, size, ratio);

	Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	PutPixelData(L, out.data);
	return 0;
}

int hist_eq(lua_State* L)
{
	double limit = lua_tonumber(L, 1);
	int glidW = lua_tointeger(L, 2);
	int glidH = lua_tointeger(L, 3);
	bool adp = static_cast<bool>(lua_toboolean(L, 4));
	bool col = static_cast<bool>(lua_toboolean(L, 5));

	auto [frame, width, height] = GetPixelData(L, "");
	Mat imgA(height, width, CV_8UC4, frame), tmpA;
	if (imgA.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	imgA.copyTo(tmpA);

	Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<Mat> bgra{ bgr, alpha };
	std::vector<int> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	Mat tmpB, tmpC;
	cvtColor(bgr, tmpB, COLOR_BGR2GRAY);
	if (adp)
	{
		auto clahe = cv::createCLAHE(limit, Size(glidW, glidH));
		clahe->apply(tmpB, tmpC);
	}
	else
	{
		cv::equalizeHist(tmpB, tmpC);
	}

	Mat out(tmpA.size(), tmpA.type());
	if (col)
	{
		parallel_for(0, height, 1, [&](int y)
			{
				auto pIN1 = tmpC.ptr<uchar>(y);
				auto pIN2 = tmpB.ptr<uchar>(y);
				auto pIN3 = tmpA.ptr<Vec4b>(y);
				auto pOUT = out.ptr<Vec4b>(y);

				for (auto x = 0; x < width; ++x)
				{
					double coef = 0;
					if (pIN2[x] != 0)
					{
						coef = ((double)pIN1[x]) / pIN2[x];
					}

					pOUT[x][0] = saturate_cast<uchar>(coef * pIN3[x][0]);
					pOUT[x][1] = saturate_cast<uchar>(coef * pIN3[x][1]);
					pOUT[x][2] = saturate_cast<uchar>(coef * pIN3[x][2]);
					pOUT[x][3] = pIN3[x][3];
				}
			}
		);
	}
	else
	{
		Mat tmpD;
		cvtColor(tmpC, tmpD, COLOR_GRAY2BGR);
		bgra[0] = tmpD.clone();
		cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	}

	PutPixelData(L, out.data);
	return 0;
}

int color_map(lua_State* L)
{
	int cmode = lua_tointeger(L, 1);
	bool hsvmode = static_cast<bool>(lua_toboolean(L, 2));
	bool invmode = static_cast<bool>(lua_toboolean(L, 3));

	auto [frame, width, height] = GetPixelData(L, "");
	Mat img(height, width, CV_8UC4, frame), tmp, tmpA;
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	std::vector<Mat> col_ch, hsv_ch;
	cv::split(tmp, col_ch);
	Mat alpha = col_ch[3].clone();

	Mat tmpB, tmpC, tmpD;
	cvtColor(tmp, tmpA, COLOR_BGRA2BGR);
	if (hsvmode)
	{
		cvtColor(tmpA, tmpB, COLOR_BGR2HSV_FULL);
		cv::split(tmpB, hsv_ch);
		hsv_ch[0].convertTo(tmpC, CV_8U);
	}
	else
	{
		cvtColor(tmpA, tmpC, COLOR_BGR2GRAY, 1);
	}
	
	if (invmode)
	{
		Mat tmp1 = Mat::ones(tmpC.size(), CV_8U) * 255;
		Mat tmp2 = Mat::zeros(tmpC.size(), CV_8U);
		cv::subtract(tmp1, tmpC, tmp2);
		tmpC = tmp2.clone();
	}

	std::vector<int> map{ COLORMAP_AUTUMN,
		COLORMAP_BONE,
		COLORMAP_JET,
		COLORMAP_WINTER,
		COLORMAP_RAINBOW,
		COLORMAP_OCEAN,
		COLORMAP_SUMMER,
		COLORMAP_SPRING,
		COLORMAP_COOL,
		COLORMAP_HSV,
		COLORMAP_PINK,
		COLORMAP_HOT,
		COLORMAP_PARULA,
		COLORMAP_MAGMA,
		COLORMAP_INFERNO,
		COLORMAP_PLASMA,
		COLORMAP_VIRIDIS,
		COLORMAP_CIVIDIS,
		COLORMAP_TWILIGHT,
		COLORMAP_TWILIGHT_SHIFTED,
		COLORMAP_TURBO,
		COLORMAP_DEEPGREEN
	};
	cv::applyColorMap(tmpC, tmpD, map[cmode]);

	parallel_for(0, img.rows, 1, [&](int y)
		{
			auto pout = img.ptr<Vec4b>(y);
			auto prgb = tmpD.ptr<Vec3b>(y);
			auto palp = alpha.ptr<uchar>(y);

			for (auto i = 0; i < img.cols; ++i)
			{
				pout[i][0] = prgb[i][0];
				pout[i][1] = prgb[i][1];
				pout[i][2] = prgb[i][2];
				pout[i][3] = palp[i];
			}
		}
	);
	PutPixelData(L, img.data);

	return 0;
}

int img_read(lua_State* L)
{
	try
	{
		auto fpath = toStringRestrict(L, 1);
		int mode = lua_tointeger(L, 2);
		double alpha = lua_tonumber(L, 3);
		double beta = lua_tonumber(L, 4);

		if (fpath == nullptr)
		{
			std::cout << "cannot read file" << "\n"
				<< "please enter/select correct value" << std::endl;
			return 0;
		}

		cv::Mat frame, img, tmp;
		if (mode == -2)
		{
			auto document = Document::loadFromFile(std::string(fpath));
			if (!document) return 0;

			uint w = constrain((int)alpha, 0, INT_MAX);
			uint h = constrain((int)beta, 0, INT_MAX);
			uint bgcol = 0x00000000;  // 背景を透明に
			auto bitmap = document->renderToBitmap(w, h, bgcol);
			bitmap.convertToRGBA();
			auto fdata = reinterpret_cast<void*>(bitmap.data());
			Mat f(Size(bitmap.width(), bitmap.height()), CV_8UC4, fdata);
			if (f.empty())
			{
				std::cout << "Mat array is empty..." << std::endl;
				return 0;
			}
			cvtColor(f, img, COLOR_BGRA2RGBA);
		}

		img_read_tmp(fpath, mode, img);
		if (img.empty())
		{
			std::cout << "Mat array is empty..." << std::endl;
			return 0;
		}

		auto dp = GetAddData("exedit.auf", 0x1b2b20);
		auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(dp);
		auto ow = exdata->obj_line;
		auto oh = exdata->obj_max_h;
		if (oh < img.rows || (ow - 8) < img.cols)
		{
			std::cout << "Mat array is too bigs..." << std::endl;
			return 0;
		}

		if (img.channels() == 3)
		{
			cvtColor(img, tmp, COLOR_BGR2BGRA);
		}
		else if (img.channels() == 1)
		{
			cvtColor(img, tmp, COLOR_GRAY2BGRA);
		}
		else
		{
			img.copyTo(tmp);
		}

		if (tmp.type() != CV_8UC4)
		{
			cv::convertScaleAbs(tmp, frame, alpha, beta);
		}
		else
		{
			tmp.copyTo(frame);
		}

		lua_getglobal(L, "obj");
		lua_getfield(L, -1, "setoption");
		lua_pushstring(L, "drawtarget");
		lua_pushstring(L, "tempbuffer");
		lua_pushinteger(L, frame.cols);
		lua_pushinteger(L, frame.rows);
		lua_call(L, 4, 0);

		lua_getglobal(L, "obj");
		lua_getfield(L, -1, "load");
		lua_pushstring(L, "tempbuffer");
		lua_call(L, 1, 0);

		PutPixelData(L, frame.data);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

int img_call(lua_State* L)
{
	cv::Mat img;
	img_read_tmp("", -1, img);
	if (img.empty())
	{
		std::cout << "Mat array is empty..." << std::endl;
		return 0;
	}

	int w = img.cols;
	int h = img.rows;
	lua_pushlightuserdata(L, img.data);
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);

	return 3;
}

int mov_read(lua_State* L)
{
	try
	{
		int idx = lua_tointeger(L, 1);
		double alpha = lua_tonumber(L, 2);
		double beta = lua_tonumber(L, 3);
		int midx = lua_tointeger(L, 4);
		bool reload = static_cast<bool>(lua_toboolean(L, 5));

		auto dp = GetAddData("exedit.auf", 0x1b2b20);
		auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(dp);
		auto tn = exdata->frame_num;
		auto ow = exdata->obj_line;
		auto oh = exdata->obj_max_h;

		std::vector<int> mode{
			cv::CAP_ANY,
			cv::CAP_FFMPEG,
			cv::CAP_IMAGES
		};

		static std::string path("");
		static std::unique_ptr<cv::VideoCapture> vcp;
		static double fnum = 0;
		static int fnum_idx = 0;
		if (reload || (path == ""))
		{
			CFileDialog fileDialog(true, nullptr, nullptr, OFN_FILEMUSTEXIST);
			if (fileDialog.DoModal() == IDOK)
			{
				CString fullPathName = fileDialog.GetPathName();
				auto huh = fullPathName.GetString();
				path = std::string(huh);

				vcp = std::make_unique<cv::VideoCapture>(path, mode[midx]);
				vcp->set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('H', '2', '6', '4'));
				fnum = vcp->get(cv::CAP_PROP_FRAME_COUNT);
				fnum_idx = static_cast<int>(fnum);
			}
		}

		if (idx == -2)
		{
			idx = tn % fnum_idx;
		}
		else if (idx == -1)
		{
			idx = constrain<int>(tn, 0, fnum_idx);
		}
		else
		{
			idx = constrain<int>(idx, 0, fnum_idx);
		}
		double val = static_cast<double>(idx);

		bool isset = vcp->set(cv::CAP_PROP_POS_FRAMES, val);
		if (!isset)
			return 0;

		cv::Mat frame, img, tmp;
		bool isread = vcp->read(img);
		if ((!isread) || img.empty())
		{
			std::cout << "Mat array is empty..." << std::endl;
			return 0;
		}

		if (oh < img.rows || (ow - 8) < img.cols)
		{
			std::cout << "Mat array is too bigs..." << std::endl;
			return 0;
		}

		if (img.channels() == 3)
		{
			cvtColor(img, tmp, cv::COLOR_BGR2BGRA);
		}
		else if (img.channels() == 1)
		{
			cvtColor(img, tmp, cv::COLOR_GRAY2BGRA);
		}
		else
		{
			img.copyTo(tmp);
		}

		if (tmp.type() != CV_8UC4)
		{
			cv::convertScaleAbs(tmp, frame, alpha, beta);
		}
		else
		{
			tmp.copyTo(frame);
		}

		lua_getglobal(L, "obj");
		lua_getfield(L, -1, "setoption");
		lua_pushstring(L, "drawtarget");
		lua_pushstring(L, "tempbuffer");
		lua_pushinteger(L, frame.cols);
		lua_pushinteger(L, frame.rows);
		lua_call(L, 4, 0);

		lua_getglobal(L, "obj");
		lua_getfield(L, -1, "load");
		lua_pushstring(L, "tempbuffer");
		lua_call(L, 1, 0);

		PutPixelData(L, frame.data);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

/********************************************************************************/
/*[utility functions]*/

template<typename T>
T constrain(T v, T min, T max)
{
	if (v < min)
	{
		v = min;
	}
	else if (max < v)
	{
		v = max;
	}
	return v;
}

void GetLuaTable(lua_State* L, int stackIndex, std::vector<std::vector<std::any>>& vals, int key)
{
	std::vector<std::any> elements;
	int stackHead = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, stackHead) != 0)
	{
		if (lua_istable(L, -1))
		{
			vals.emplace_back(elements);
			GetLuaTable(L, lua_gettop(L), vals, key + 1);
		}
		else
		{
			std::any ele = nullptr;
			bool flags = false;
			switch (lua_type(L, -1))
			{
			case LUA_TBOOLEAN:
				ele = static_cast<bool>(lua_toboolean(L, -1));
				flags = true;
				break;
			case LUA_TNUMBER:
				ele = lua_tonumber(L, -1);
				flags = true;
				break;
			case LUA_TSTRING:
				ele = toStringRestrict(L, -1);
				flags = (typeid(ele) == typeid(const char*));
				break;
			case LUA_TLIGHTUSERDATA:
				ele = lua_touserdata(L, -1);
				flags = true;
				break;
			case LUA_TUSERDATA:
				ele = lua_topointer(L, -1);
				flags = true;
				break;
			default:
				break;
			}

			if (flags)
				vals[key].emplace_back(ele);
		}
		lua_pop(L, 1);
	}
}

const char* toStringRestrict(lua_State* L, int idx) noexcept
{
	if (lua_isnumber(L, idx))
	{
		double num = lua_tonumber(L, idx);
		std::string tmp = std::to_string(num);
		const char* chr = tmp.c_str();
		return chr;
	}
	else if (lua_isstring(L, idx))
	{
		return lua_tostring(L, idx);
	}
	else
	{
		return nullptr;
	}
}

uintptr_t GetAddData(const char* modname, uintptr_t address)
{
	HMODULE hmod = GetModuleHandleA(modname);
	uint32_t& base_add = (uint32_t&)hmod;
	uint32_t dp = *std::bit_cast<std::add_pointer_t<uint32_t>>(base_add + address);

	DWORD old = 0;
	auto pt = reinterpret_cast<LPVOID>(dp);
	VirtualProtect(pt, sizeof(pt), PAGE_EXECUTE_READWRITE, &old);
	return dp;
}

std::tuple<void*, int, int> GetPixelData(lua_State* L, const char* mode)
{
	lua_getglobal(L, "obj");
	lua_getfield(L, -1, "getpixeldata");
	lua_pushstring(L, mode);  // ここから引数
	lua_call(L, 1, 3);

	void* framedata = lua_touserdata(L, -3);
	int width = lua_tointeger(L, -2);
	int height = lua_tointeger(L, -1);

	lua_pop(L, 3);  // スタックから消去

	return { framedata, width, height };
}

void PutPixelData(lua_State* L, void* pixeldata)
{
	lua_getglobal(L, "obj");
	lua_getfield(L, -1, "putpixeldata");
	lua_pushlightuserdata(L, pixeldata);  // ここから引数
	lua_call(L, 1, 0);
	lua_pop(L, 1);
}

void img_read_tmp(String path, int idx, Mat& out)
{
	static Mat img;
	std::vector<int> mode{
		IMREAD_UNCHANGED,
		IMREAD_COLOR | IMREAD_ANYDEPTH,
		IMREAD_GRAYSCALE | IMREAD_ANYDEPTH,
		IMREAD_REDUCED_COLOR_2,
		IMREAD_REDUCED_COLOR_4,
		IMREAD_REDUCED_COLOR_8
	};

	if (0 <= idx)
	{
		img = cv::imread(path, mode[idx]);
	}
	else if (idx == -2)
	{
		img = out.clone();
		return;
	}
	out = img.clone();
}
