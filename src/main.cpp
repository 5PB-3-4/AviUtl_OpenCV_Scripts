#include "pch.h"
#include <berl_util_module.hpp>

#pragma warning(disable:4244)
#pragma warning(disable:4819)
#pragma warning(disable:26451)

/********************************************************************************/
/*[prototype]*/

// main function
int32_t medianblur(lua_State* L);
int32_t boxblur(lua_State* L);
int32_t gaussianblur(lua_State* L);
int32_t bilateralfilter(lua_State* L);
int32_t resizefilter(lua_State* L);
int32_t resample(lua_State* L);
int32_t sobelfilter(lua_State* L);
int32_t sobelRGB(lua_State* L);
int32_t laplacianfilter(lua_State* L);
int32_t laplacianRGB(lua_State* L);
int32_t cannyfilter(lua_State* L);
int32_t cannyRGB(lua_State* L);
int32_t morphologyfilter(lua_State* L);
int32_t morphologyRGB(lua_State* L);
int32_t ch_chg(lua_State* L);
int32_t thres(lua_State* L);
int32_t adpthres(lua_State* L);
int32_t diffusion(lua_State* L);
int32_t noise(lua_State* L);
int32_t fig_blur(lua_State* L);
int32_t droste(lua_State* L);
int32_t moebius(lua_State* L);
int32_t moebius2(lua_State* L);
int32_t meanshift(lua_State* L);
int32_t voronoi(lua_State* L);
int32_t delaunay(lua_State* L);
int32_t pencilskc(lua_State* L);
int32_t stylize(lua_State* L);
int32_t det_enhance(lua_State* L);
int32_t edgePreserve(lua_State* L);
int32_t oilpaint(lua_State* L);
int32_t hist_eq(lua_State* L);
int32_t color_map(lua_State* L);
int32_t img_read(lua_State* L);
int32_t img_call(lua_State* L);
int32_t mov_read(lua_State* L);

// utility function
static inline double modulo(double x, double y);
void img_read_tmp(cv::String path, int32_t idx, cv::Mat& out);

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
	{"droste", droste},
	{"moebius", moebius},
	{"moebius2", moebius2},
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
	__declspec(dllexport) int32_t luaopen_cvmods(lua_State* L)
	{
		luaL_register(L, "cvmods", functions);
		return 1;
	}
}

/********************************************************************************/
/*[callback function]*/

int32_t medianblur(lua_State* L)
{
	int32_t ksize = lua_tointeger(L, 1);

	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmp, out;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	cv::medianBlur(tmp, out, 2 * ksize + 1);

	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t boxblur(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t boxW = lua_tointeger(L, 1);
	int32_t boxH = lua_tointeger(L, 2);
	bool Normalize = static_cast<bool>(lua_toboolean(L, 3));
	int32_t border = lua_tointeger(L, 4);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmp, out;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	std::vector<int32_t> mode{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_REFLECT_101,
		cv::BORDER_ISOLATED
	};

	cv::boxFilter(tmp, out, -1, cv::Size(boxW, boxH), cv::Point(-1, -1), Normalize, mode[border]);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t gaussianblur(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t boxW = lua_tointeger(L, 1);
	int32_t boxH = lua_tointeger(L, 2);
	double sigmaX = lua_tonumber(L, 3);
	double sigmaY = lua_tonumber(L, 4);
	int32_t border = lua_tointeger(L, 5);
	bool alphamask = static_cast<bool>(lua_toboolean(L, 6));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmp, out;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	std::vector<int32_t> mode{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_REFLECT_101,
		cv::BORDER_ISOLATED
	};

	int32_t kx = std::max(2 * boxW - 1, 0);
	int32_t ky = std::max(2 * boxH - 1, 0);
	cv::GaussianBlur(tmp, out, cv::Size(kx, ky), sigmaX, sigmaY, mode[border]);

	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t bilateralfilter(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t d = lua_tointeger(L, 1);
	double sigmacolor = lua_tonumber(L, 2);
	double sigmaspace = lua_tonumber(L, 3);
	int32_t border = lua_tointeger(L, 4);
	bool alphasync = static_cast<bool>(lua_toboolean(L, 5));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmp;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);


	cv::Mat bgr(tmp.size(), CV_8UC3), alpha(tmp.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmp, 1, bgra.data(), 2, ch.data(), 4);

	std::vector<int32_t> mode{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_WRAP,
		cv::BORDER_REFLECT_101
	};

	cv::Mat tmpB, tmpC;
	cv::bilateralFilter(bgra[0], tmpB, d, sigmacolor, sigmaspace, mode[border]);
	bgra[0] = tmpB.clone();
	if (alphasync)
	{
		cv::bilateralFilter(bgra[1], tmpC, d, sigmacolor, sigmaspace, mode[border]);
		bgra[1] = tmpC.clone();
	}


	cv::Mat out(tmp.size(), img.type());
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);

	utils->PutPixelData(L, out.data);

	return 0;
}

int32_t resizefilter(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t new_w = lua_tointeger(L, 1);
	int32_t new_h = lua_tointeger(L, 2);
	double coef_w = lua_tonumber(L, 3);
	double coef_h = lua_tonumber(L, 4);
	int32_t resize_mode = lua_tointeger(L, 5);
	bool set_size = static_cast<bool>(lua_toboolean(L, 6));

	auto dp = utils->GetAddData("exedit.auf", 0x1b2b20);
	auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(dp);
	auto dt = exdata->obj_edit;
	auto wt = exdata->obj_w;
	auto ht = exdata->obj_h;
	auto ow = exdata->obj_line;
	auto oh = exdata->obj_max_h;

	cv::Mat buf_bgra = cv::Mat::zeros(cv::Size(wt, ht), CV_8UC4);
	utils->yca2mat(&dt, buf_bgra, ow);

	std::vector<int32_t> rsmode{
		cv::INTER_NEAREST,
		cv::INTER_LINEAR,
		cv::INTER_CUBIC,
		cv::INTER_AREA,
		cv::INTER_LANCZOS4,
		cv::INTER_LINEAR_EXACT,
		cv::INTER_NEAREST_EXACT
	};

	cv::Mat out;
	if (set_size)
	{
		new_w = std::clamp(new_w, 0, ow - 8);
		new_h = std::clamp(new_h, 0, oh);
		cv::resize(buf_bgra, out, cv::Size(new_w, new_h), 0.0, 0.0, rsmode[resize_mode]);
	}
	else
	{
		double cw_max = static_cast<double>(ow - 8) / wt;
		double ch_max = static_cast<double>(oh) / ht;
		coef_w = std::clamp(coef_w, 0.0, cw_max);
		coef_h = std::clamp(coef_h, 0.0, ch_max);
		cv::resize(buf_bgra, out, cv::Size(), coef_w, coef_h, rsmode[resize_mode]);
		new_w = out.cols;
		new_h = out.rows;
	}

	if ((ow - 8) < new_w || oh < new_h)
	{
		std::cout << "image is  too big..." << std::endl;
		return 0;
	}

	utils->mat2yca(out, &dt, &exdata, ow);
	return 0;
}

int32_t resample(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double coef_w = lua_tonumber(L, 1);
	double coef_h = lua_tonumber(L, 2);
	int32_t resize_exp = lua_tointeger(L, 3);
	int32_t resize_red = lua_tointeger(L, 4);

	auto dp = utils->GetAddData("exedit.auf", 0x1b2b20);
	auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(dp);
	auto dt = exdata->obj_edit;
	auto wt = exdata->obj_w;
	auto ht = exdata->obj_h;
	auto ow = exdata->obj_line;
	auto oh = exdata->obj_max_h;

	cv::Mat buf_bgra = cv::Mat::zeros(cv::Size(wt, ht), CV_8UC4);
	utils->yca2mat(&dt, buf_bgra, ow);
	
	cv::Mat eximg, rdimg;
	std::vector<int32_t> rsmode{
		cv::INTER_NEAREST,
		cv::INTER_LINEAR,
		cv::INTER_CUBIC,
		cv::INTER_AREA,
		cv::INTER_LANCZOS4,
		cv::INTER_LINEAR_EXACT,
		cv::INTER_NEAREST_EXACT
	};

	double cw_max = static_cast<double>(ow - 8) / wt;
	double ch_max = static_cast<double>(oh) / ht;
	coef_w = std::clamp(0.01 * coef_w, 0.0, cw_max);
	coef_h = std::clamp(0.01 * coef_h, 0.0, ch_max);
	cv::resize(buf_bgra, rdimg, cv::Size(), coef_w, coef_h, rsmode[resize_red]);

	double cx = static_cast<double>(wt) / rdimg.cols;
	double cy = static_cast<double>(ht) / rdimg.rows;
	cv::resize(rdimg, eximg, cv::Size(), cx, cy, rsmode[resize_exp]);

	int32_t new_w = eximg.cols;
	int32_t new_h = eximg.rows;

	if ((ow - 8) < new_w || oh < new_h)
	{
		std::cout << "image is  too big..." << std::endl;
		return 0;
	}

	utils->mat2yca(eximg, &dt, &exdata, ow);
	return 0;
}

int32_t sobelfilter(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t ksize = lua_tointeger(L, 1);
	bool balp = static_cast<bool>(lua_toboolean(L, 2));
	int32_t dx = lua_tointeger(L, 3);
	int32_t dy = lua_tointeger(L, 4);
	int32_t border = lua_tointeger(L, 5);
	double scale = lua_tonumber(L, 6);
	double delta = lua_tonumber(L, 7);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Mat tmpB, tmpC, tmpD, tmpE;
	cv::cvtColor(bgr, tmpB, cv::COLOR_BGR2GRAY);
	tmpB.convertTo(tmpC, CV_32F, 1.0 / 255, 0.0);

	std::vector<int32_t> mode
	{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_REFLECT_101,
		cv::BORDER_ISOLATED
	};

	ksize = std::max(2 * ksize - 1, 0);
	cv::Sobel(tmpC, tmpD, -1, dx, dy, ksize, scale, delta, mode[border]);
	cv::convertScaleAbs(tmpD, tmpE, 255.0, 0.0);

	if (balp)
		cv::threshold(tmpE, bgra[1], 0.1, 255, cv::THRESH_BINARY);

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpE, tmpE, tmpE, bgra[1] };
	cv::merge(ch_out, out);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t sobelRGB(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t ksize = lua_tointeger(L, 1);
	bool balp = static_cast<bool>(lua_toboolean(L, 2));
	int32_t dx = lua_tointeger(L, 3);
	int32_t dy = lua_tointeger(L, 4);
	int32_t border = lua_tointeger(L, 5);
	double scale = lua_tonumber(L, 6);
	double delta = lua_tonumber(L, 7);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA, tmpB;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);
	tmpA.convertTo(tmpB, CV_32FC4, 1.0 / 255, 0.0);

	std::vector<cv::Mat> ch_col;
	cv::split(tmpB, ch_col);

	std::vector<int32_t> mode
	{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_REFLECT_101,
		cv::BORDER_ISOLATED
	};

	ksize = std::max(2 * ksize - 1, 0);
	cv::Mat tmpC = cv::Mat::zeros(tmpA.size(), CV_32F);
	cv::Mat tmpD, tmpF;
	for (auto i = 0; i < 3; ++i)
	{
		cv::Mat tmpE;
		cv::Sobel(ch_col[i], tmpE, -1, dx, dy, ksize, scale, delta, mode[border]);
		cv::add(tmpC, tmpE, tmpC);
	}
	cv::convertScaleAbs(tmpC, tmpD, 255.0, 0.0);
		
	if (balp)
		cv::threshold(tmpD, tmpF, 0.1, 255, cv::THRESH_BINARY);
	else
		cv::convertScaleAbs(ch_col[3], tmpF, 255.0, 0.0);

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpD, tmpD, tmpD, tmpF };
	cv::merge(ch_out, out);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t laplacianfilter(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t ksize = lua_tointeger(L, 1);
	bool balp = static_cast<bool>(lua_toboolean(L, 2));
	int32_t border = lua_tointeger(L, 3);
	double scale = lua_tonumber(L, 4);
	double delta = lua_tonumber(L, 5);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Mat tmpB, tmpC, tmpD, tmpE;
	cv::cvtColor(tmpA, tmpB, cv::COLOR_BGRA2GRAY);
	tmpB.convertTo(tmpC, CV_32F, 1.0 / 255.0, 0.0);

	std::vector<int32_t> mode{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_REFLECT_101,
		cv::BORDER_ISOLATED
	};

	ksize = std::max(2 * ksize - 1, 0);
	cv::Laplacian(tmpC, tmpD, -1, ksize, scale, delta, mode[border]);
	cv::convertScaleAbs(tmpD, tmpE, 255.0, 0.0);

	if (balp)
		cv::threshold(tmpE, bgra[1], 0.1, 255, cv::THRESH_BINARY);

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpE, tmpE, tmpE, bgra[1] };
	cv::merge(ch_out, out);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t laplacianRGB(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t ksize = lua_tointeger(L, 1);
	bool balp = static_cast<bool>(lua_toboolean(L, 2));
	int32_t border = lua_tointeger(L, 3);
	double scale = lua_tonumber(L, 4);
	double delta = lua_tonumber(L, 5);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA, tmpB;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);
	tmpA.convertTo(tmpB, CV_32FC4, 1.0 / 255, 0.0);

	std::vector<cv::Mat> ch_col;
	cv::split(tmpB, ch_col);

	std::vector<int32_t> mode{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_REFLECT_101,
		cv::BORDER_ISOLATED
	};
	ksize = std::max(2 * ksize - 1, 0);

	cv::Mat tmpC = cv::Mat::zeros(tmpA.size(), CV_32F);
	cv::Mat tmpD, tmpF;
	for (auto i = 0; i < 3; ++i)
	{
		cv::Mat tmpE;
		cv::Laplacian(ch_col[i], tmpE, -1, ksize, scale, delta, mode[border]);
		cv::add(tmpC, tmpE, tmpC);
	}
	cv::convertScaleAbs(tmpC, tmpD, 255.0, 0.0);

	if (balp)
		cv::threshold(tmpD, tmpF, 0.1, 255, cv::THRESH_BINARY);
	else
		cv::convertScaleAbs(ch_col[3], tmpF, 255.0, 0.0);

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpD, tmpD, tmpD, tmpF };
	cv::merge(ch_out, out);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t cannyfilter(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double thres1 = lua_tonumber(L, 1);
	double thres2 = lua_tonumber(L, 2);
	int32_t asize = lua_tointeger(L, 3);
	bool gradient = static_cast<bool>(lua_toboolean(L, 4));
	bool balp = static_cast<bool>(lua_toboolean(L, 5));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Mat tmpB, tmpC;
	cv::cvtColor(bgr, tmpB, cv::COLOR_BGRA2GRAY);
	cv::Canny(tmpB, tmpC, thres1, thres2, 2 * asize - 1, gradient);

	if (balp)
		cv::threshold(tmpC, bgra[1], 0.1, 255, cv::THRESH_BINARY);

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpC, tmpC, tmpC, bgra[1] };
	cv::merge(ch_out, out);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t cannyRGB(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double thres1 = lua_tonumber(L, 1);
	double thres2 = lua_tonumber(L, 2);
	int32_t asize = lua_tointeger(L, 3);
	bool gradient = static_cast<bool>(lua_toboolean(L, 4));
	bool balp = static_cast<bool>(lua_toboolean(L, 5));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	std::vector<cv::Mat> ch_col;
	cv::split(tmpA, ch_col);

	cv::Mat tmpB = cv::Mat::zeros(tmpA.size(), CV_8U);
	for (auto i = 0; i < 3; ++i)
	{
		cv::Mat tmpC;
		cv::Canny(ch_col[i], tmpC, thres1, thres2, 2 * asize - 1, gradient);
		cv::add(tmpB, tmpC, tmpB);
	}

	if (balp)
		cv::threshold(tmpB, ch_col[3], 0.1, 255, cv::THRESH_BINARY);

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpB, tmpB, tmpB, ch_col[3] };
	cv::merge(ch_out, out);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t morphologyfilter(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t ksize = lua_tointeger(L, 1);
	int32_t ktype = lua_tointeger(L, 2);
	int32_t mode = lua_tointeger(L, 3);
	int32_t iterations = lua_tointeger(L, 4);
	int32_t border = lua_tointeger(L, 5);
	bool alphasync = static_cast<bool>(lua_toboolean(L, 6));

	luaL_openlibs(L);
	lua_getglobal(L, "anc");
	int32_t stackidx = lua_gettop(L);
	lua_pushnil(L);
	std::array<int32_t, 2> pt{ 0, 0 };
	int32_t j = 0;
	while (lua_next(L, stackidx) != 0)
	{
		if (lua_isnumber(L, -1))
		{
			pt[j] = lua_tointeger(L, -1);
			++j;
		}
		lua_pop(L, 1);
	}

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	std::vector<int32_t> kshape{
		cv::MORPH_RECT,
		cv::MORPH_CROSS,
		cv::MORPH_ELLIPSE
	};

	std::vector<int32_t> opmode{
		cv::MORPH_ERODE,
		cv::MORPH_DILATE,
		cv::MORPH_OPEN,
		cv::MORPH_CLOSE,
		cv::MORPH_GRADIENT,
		cv::MORPH_TOPHAT,
		cv::MORPH_BLACKHAT,
		cv::MORPH_HITMISS
	};

	std::vector<int32_t> bmode{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_REFLECT_101,
		cv::BORDER_ISOLATED
	};

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	img.copyTo(tmpA);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Mat tmpB = cv::Mat::zeros(tmpA.size(), CV_8UC3);
	cv::Mat tmpC, tmpD;
	
	bgra[0].copyTo(tmpB, bgra[1]);
	cv::cvtColor(tmpB, tmpB, cv::COLOR_BGR2GRAY);
	ksize = std::max(2 * ksize - 1, 1);
	cv::Point anchor(pt[0], pt[1]);
	cv::Mat kernel = getStructuringElement(kshape[ktype], cv::Size(ksize, ksize), anchor);
	cv::morphologyEx(tmpB, tmpC, opmode[mode], kernel, anchor, iterations, bmode[border]);

	if (alphasync)
		cv::morphologyEx(bgra[1], tmpD, opmode[mode], kernel, anchor, iterations, bmode[border]);
	else
		tmpD = bgra[1].clone();

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpC, tmpC, tmpC, tmpD };
	cv::merge(ch_out, out);
	utils->PutPixelData(L, out.data);

	return 0;
}

int32_t morphologyRGB(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t ksizeB = lua_tointeger(L, 1);
	int32_t ksizeG = lua_tointeger(L, 2);
	int32_t ksizeR = lua_tointeger(L, 3);
	int32_t ktype = lua_tointeger(L, 4);
	int32_t mode = lua_tointeger(L, 5);
	int32_t iterations = lua_tointeger(L, 6);
	int32_t border = lua_tointeger(L, 7);
	bool alphasync = static_cast<bool>(lua_toboolean(L, 8));

	luaL_openlibs(L);
	lua_getglobal(L, "anc");
	int32_t stackidx = lua_gettop(L);
	lua_pushnil(L);
	std::array<int32_t, 2> pt{ 0, 0 };
	int32_t j = 0;
	while (lua_next(L, stackidx) != 0)
	{
		if (lua_isnumber(L, -1))
		{
			pt[j] = lua_tointeger(L, -1);
			++j;
		}
		lua_pop(L, 1);
	}

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat tmpB = cv::Mat::zeros(tmpA.size(), CV_8UC3);
	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	img.copyTo(tmpA);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);
	bgra[0].copyTo(tmpB, bgra[1]);

	std::vector<cv::Mat> col_ch;
	cv::split(tmpB, col_ch);

	std::vector<int32_t> kshape{
		cv::MORPH_RECT,
		cv::MORPH_CROSS,
		cv::MORPH_ELLIPSE
	};

	std::vector<int32_t> opmode{
		cv::MORPH_ERODE,
		cv::MORPH_DILATE,
		cv::MORPH_OPEN,
		cv::MORPH_CLOSE,
		cv::MORPH_GRADIENT,
		cv::MORPH_TOPHAT,
		cv::MORPH_BLACKHAT,
		cv::MORPH_HITMISS
	};

	std::vector<int32_t> bmode{
		cv::BORDER_CONSTANT,
		cv::BORDER_REPLICATE,
		cv::BORDER_REFLECT,
		cv::BORDER_REFLECT_101,
		cv::BORDER_ISOLATED
	};

	std::vector<int32_t> ksize{ ksizeB, ksizeG, ksizeR };
	cv::Point anchor(pt[0], pt[1]);
	std::vector<cv::Mat> kernel;
	for (auto i = 0; i < 3; ++i)
	{
		cv::Mat tmpB = getStructuringElement(kshape[ktype], cv::Size(2 * ksize[i] - 1, 2 * ksize[i] - 1), anchor);
		kernel.emplace_back(tmpB);
		col_ch.emplace_back(bgra[1]);
	}

	cv::Mat tmpC = cv::Mat::zeros(tmpA.size(), CV_8U);
	std::vector<cv::Mat> tmpD(6, tmpC);
	iterations = std::max(iterations, 1);
	Concurrency::parallel_for(0, 6, 1, [&](int32_t j)
		{
			cv::Mat tmpE;
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

	cv::Mat out;
	std::vector ch_out{ tmpD[0], tmpD[1], tmpD[2], tmpC};
	cv::merge(ch_out, out);

	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t ch_chg(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t ch_red = lua_tointeger(L, 1);
	int32_t ch_green = lua_tointeger(L, 2);
	int32_t ch_blue = lua_tointeger(L, 3);
	int32_t ch_alpha = lua_tointeger(L, 4);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmp;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	cv::Mat out(tmp.size(), tmp.type());
	std::vector<int32_t> ch{ ch_blue,0, ch_green,1, ch_red,2, ch_alpha,3 };
	cv::mixChannels(&tmp, 1, &out, 1, ch.data(), 4);

	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t thres(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double thres = lua_tonumber(L, 1);
	double maxv = lua_tonumber(L, 2);
	int32_t tmode = lua_tointeger(L, 3);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);
	
	std::vector<int32_t> mode{
		cv::THRESH_BINARY,
		cv::THRESH_BINARY_INV,
		cv::THRESH_TRUNC,
		cv::THRESH_TOZERO,
		cv::THRESH_TOZERO_INV,
		cv::THRESH_OTSU,
		cv::THRESH_TRIANGLE
	};

	cv::Mat tmpB, tmpC;
	cv::cvtColor(bgr, tmpB, cv::COLOR_BGR2GRAY);
	cv::threshold(tmpB, tmpC, thres, maxv, mode[tmode]);

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpC, tmpC, tmpC, bgra[1] };
	cv::merge(ch_out, out);

	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t adpthres(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t adpthres = lua_tointeger(L, 1);
	double maxv = lua_tonumber(L, 2);
	int32_t bsize = lua_tointeger(L, 3);
	int32_t tmode = lua_tointeger(L, 4);
	double valC = lua_tonumber(L, 5);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA, tmpB;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);
	cv::cvtColor(bgra[0], tmpB, cv::COLOR_BGR2GRAY);

	std::vector<int32_t> mode{
		cv::THRESH_BINARY_INV,
		cv::THRESH_BINARY
	};

	std::vector<int32_t> adp_mode{
		cv::ADAPTIVE_THRESH_MEAN_C,
		cv::ADAPTIVE_THRESH_GAUSSIAN_C
	};

	cv::Mat tmpC;
	tmode = std::clamp(tmode, 0, 1);
	adpthres = std::clamp(adpthres, 0, 1);
	cv::adaptiveThreshold(tmpB, tmpC, maxv, adp_mode[adpthres], mode[tmode], 2 * bsize + 1, valC);

	cv::Mat out;
	std::vector<cv::Mat> ch_out{ tmpC, tmpC, tmpC, bgra[1] };
	cv::merge(ch_out, out);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t diffusion(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t iter = lua_tointeger(L, 1);
	float decay_factor = static_cast<float>(lua_tonumber(L, 2));
	float decay_offset = static_cast<float>(lua_tonumber(L, 3));
	float gamma = static_cast<float>(lua_tonumber(L, 4));
	int32_t sigma = std::max(lua_tointeger(L, 5), 0);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA, tmpB;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);
	tmpA.convertTo(tmpB, CV_32FC4, 1.0 / 255.0);

	cv::Mat tmpC = cv::Mat::zeros(tmpB.size(), CV_32FC3);
	Concurrency::parallel_for(0, height, 1, [&](int32_t y)
		{
			auto pIN = tmpB.ptr<cv::Vec4f>(y);
			auto pPOW = tmpC.ptr<cv::Vec3f>(y);

			for (auto x = 0; x < width; ++x)
			{
				pPOW[x][0] = std::pow(pIN[x][0], gamma);
				pPOW[x][1] = std::pow(pIN[x][1], gamma);
				pPOW[x][2] = std::pow(pIN[x][2], gamma);
			}
		}
	);

	cv::Mat tmpE = tmpC.clone();
	for (int32_t i = 0; i < iter; ++i)
	{
		float gain = std::pow(decay_factor, -((float)i + decay_offset));
		sigma *= sigma;
		cv::Mat tmpD;
		cv::GaussianBlur(tmpC, tmpD, cv::Size(), (double)sigma);

		Concurrency::parallel_for(0, height, 1, [&tmpD, &tmpE, gain, width](int32_t y)
			{
				auto pIN1 = tmpD.ptr<cv::Vec3f>(y);
				auto pIN2 = tmpE.ptr<cv::Vec3f>(y);

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
	cv::Mat tmpF = cv::Mat::zeros(tmpB.size(), tmpB.type());
	Concurrency::parallel_for(0, height, 1, [&tmpE, &tmpF, &tmpB, gm_inv, width](int32_t y)
		{
			auto pIN3 = tmpE.ptr<cv::Vec3f>(y);
			auto pIN4 = tmpB.ptr<cv::Vec4f>(y);
			auto pOUT = tmpF.ptr<cv::Vec4f>(y);

			for (auto x = 0; x < width; ++x)
			{
				pOUT[x][0] = std::clamp(std::pow(pIN3[x][0], gm_inv), 0.f, 1.f);
				pOUT[x][1] = std::clamp(std::pow(pIN3[x][1], gm_inv), 0.f, 1.f);
				pOUT[x][2] = std::clamp(std::pow(pIN3[x][2], gm_inv), 0.f, 1.f);
				pOUT[x][3] = pIN4[x][3];
			}
		}
	);

	cv::Mat out;
	convertScaleAbs(tmpF, out, 255.0, 0.0);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t noise(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	float alp = static_cast<float>(lua_tonumber(L, 1));
	float valK = static_cast<float>(lua_tonumber(L, 2));
	int32_t iter = lua_tointeger(L, 3);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Mat tmpB, out(tmpA.size(), tmpA.type());
	cv::ximgproc::anisotropicDiffusion(bgra[0], tmpB, alp, valK, iter);
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t fig_blur(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double size = lua_tonumber(L, 1);
	double cbrt = lua_tonumber(L, 2);
	auto fpath = utils->toStringRestrict(L, 3);
	
	try
	{
		cv::Mat k1 = cv::imread(fpath, cv::IMREAD_GRAYSCALE);

		// size : sy = col : rows
		int32_t sx = static_cast<int32_t>(size);
		int32_t sy = static_cast<int32_t>(double(sx) * k1.rows / k1.cols);
		cv::Mat tmpA;
		cv::resize(k1, tmpA, cv::Size(sx, sy), 0.0, 0.0, cv::INTER_LANCZOS4);

		cv::Mat tmpB, tmpC, kernel;
		cv::Mat tmpD = cv::Mat::ones(tmpA.size(), CV_32F);
		tmpA.convertTo(tmpB, CV_32F, 1.0, 0.0);
		cv::Scalar sums = cv::sum(tmpB);
		cv::divide(tmpB, tmpD * sums[0], tmpC);
		cv::multiply(tmpC, tmpD*cbrt, kernel);

		auto [frame, width, height] = utils->GetPixelData(L, "");
		cv::Mat img(height, width, CV_8UC4, frame), in0;
		if (img.empty())
		{
			std::cout << "cv::Mat array is empty..." << std::endl;
			return 0;
		}
		img.copyTo(in0);

		cv::Mat bgr(img.size(), CV_8UC3), alpha(img.size(), CV_8U);
		std::vector<cv::Mat> bgra{ bgr, alpha };
		std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
		cv::mixChannels(&in0, 1, bgra.data(), 2, ch.data(), 4);
		
		cv::Mat in1, in2, out1, out2;
		bgra[0].convertTo(in1, CV_32F);
		bgra[1].convertTo(in2, CV_32F);
		cv::filter2D(in1, out1, -1, kernel);
		cv::filter2D(in2, out2, -1, kernel);

		cv::Mat out3(img.size(), CV_8UC4);
		cv::convertScaleAbs(out1, bgra[0], 1.0, 0.0);
		cv::convertScaleAbs(out2, bgra[1], 1.0, 0.0);
		cv::mixChannels(bgra.data(), 2, &out3, 1, ch.data(), 4);
		utils->PutPixelData(L, out3.data);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

int32_t droste(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double angle = lua_tonumber(L, 1);
	int32_t convertType = std::clamp(lua_tointeger(L, 2), 0, 2);
	double periodicity = lua_tonumber(L, 3);
	double strand = lua_tonumber(L, 4);
	bool transparent = static_cast<bool>(lua_toboolean(L, 5));
	double r1 = lua_tonumber(L, 6);
	double r2 = lua_tonumber(L, 7);
	double ratio = lua_tonumber(L, 8);
	double rectRatioW = lua_tonumber(L, 9);
	double rectRatioH = lua_tonumber(L, 10);
	bool useRect = static_cast<bool>(lua_toboolean(L, 11));
	int32_t shepemethod = std::clamp(lua_tointeger(L, 12), 0, 6);

	auto efpip = utils->GetAddData("exedit.auf", 0x1b2b20);
	auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(efpip);
	auto dt = exdata->obj_edit;
	auto wt = exdata->obj_w;
	auto ht = exdata->obj_h;
	auto ow = exdata->obj_line;
	auto oh = exdata->obj_max_h;
	auto obj_data = exdata->obj_data;
	if ((ow < 1) || (oh < 1) || (wt < 1) || (ht < 1))
		return 0;

	cv::Mat buf(cv::Size(wt, ht), CV_8UC4);
	utils->yca2mat(&dt, buf, ow);
	if (buf.empty())
		return 0;

	std::vector<std::vector<double>> focus = utils->GetLuaTableNum(L, "centerpoint");
	auto cfx = focus[1][0] + 0.5 * wt;
	auto cfy = focus[1][1] + 0.5 * ht;

	constexpr std::complex<double> I(0.0, 1.0);
	try
	{
		double div = r2 / r1;
		double logR2R1 = log(div);
		double logm  = log(div);
		double logR1 = log(r1);
		double angleDiff = angle * CV_PI / 180.0;

		double tmp;
		switch (convertType) {
		case 1:
			tmp = atan(logR2R1 * strand / periodicity / (2.0 * CV_PI));
			break;
		case 2:
			tmp = CV_PI - atan(logR2R1 * strand / periodicity / (2.0 * CV_PI));
			break;
		default:
			tmp = 0.0;
			break;
		}
		double alpha = tmp;

		double cosAlpha = cos(alpha);
		double sinAlpha = sin(alpha);
		double f = (2.0 * CV_PI) / (2.0 * CV_PI - angleDiff) * cosAlpha;

		double angle_ = -angle * CV_PI / 180.0;
		double cosAngle = cos(angle_);
		double sinAngle = sin(angle_);
		ratio = 1.0 / pow(2.0, ratio);

		double lx1 = r1 * rectRatioW;
		double lx2 = r2 * rectRatioW;
		double ly1 = r1 * rectRatioH;
		double ly2 = r2 * rectRatioH;
		double scale = r2 / r1;

		cv::Mat map(cv::Size(wt, ht), CV_32FC2), map2;
		int32_t mapdiff = 0;
		if (transparent)
		{
			map2 = cv::Mat(cv::Size(wt, ht), CV_32FC2);
			mapdiff = map2.ptr<float>(0) - map.ptr<float>(0);
		}

		Concurrency::parallel_for(0, ht, 1, [&](int32_t iy)
			{
				float* yp = map.ptr<float>(iy);
				double yy = iy - cfy;

				for (auto ix = 0; ix < wt; ++ix)
				{
					double xx = ix - cfx;
					double x = (xx * cosAngle + yy * sinAngle) * ratio;
					double y = (xx * -sinAngle + yy * cosAngle) * ratio;

					double r = sqrt(x * x + y * y);
					double logR = log(r) - logR1;
					double theta = atan2(y, x) + angleDiff * (logR / logR2R1);

					logR *= periodicity * strand;
					theta *= periodicity;

					double s = (logR * cosAlpha - theta * sinAlpha) / f;
					double t = (theta * cosAlpha + logR * sinAlpha) / f;

					double ms = modulo(s, logR2R1);
					double mt = modulo(t, 2.0 * CV_PI);

					double p = r1 * exp(ms) * cos(mt);
					double q = r1 * exp(ms) * sin(mt);

					if (useRect) {
						if ((abs(p) > lx2 || abs(q) > ly2)) {
							p /= scale;
							q /= scale;
						}
						else if ((abs(p) < lx1 && abs(q) < ly1)) {
							p *= scale;
							q *= scale;
						}
					}

					double mx = p + cfx;
					double my = q + cfy;

					yp[0] = (float)mx;
					yp[1] = (float)my;

					if (transparent) {
						p /= scale;
						q /= scale;
						yp[mapdiff] = (float)(p + cfx);
						yp[mapdiff + 1] = (float)(q + cfy);
					}

					yp += 2;
				}
			}
		);

		cv::Mat dst(buf.size(), CV_8UC4);
		std::vector<int32_t> rsmode{
			cv::INTER_NEAREST,
			cv::INTER_LINEAR,
			cv::INTER_CUBIC,
			cv::INTER_AREA,
			cv::INTER_LANCZOS4,
			cv::INTER_LINEAR_EXACT,
			cv::INTER_NEAREST_EXACT
		};
		if (transparent)
		{
			cv::remap(buf, dst, map2, cv::Mat(), rsmode[shepemethod], 0);

			cv::Mat img;
			cv::remap(buf, img, map, cv::Mat(), rsmode[shepemethod], 0);

			Concurrency::parallel_for(0, buf.rows, 1, [&](int32_t y)
				{
					byte* yp1 = img.ptr<byte>(y);
					byte* yp2 = dst.ptr<byte>(y);

					for (int32_t x = 0; x < buf.cols; ++x)
					{
						byte* xp1 = yp1 + x * 4;
						byte* xp2 = yp2 + x * 4;
						byte a1 = xp1[3];

						if (a1 == 255) {
							xp2[0] = xp1[0];
							xp2[1] = xp1[1];
							xp2[2] = xp1[2];
							xp2[3] = xp1[3];
						}
						else if (a1 == 0) {
							;
						}
						else {
							int32_t a2 = xp2[3];
							int32_t newA = 255 * 255 - (255 - a1) * (255 - a2);
							int32_t c[3] = {
								(xp1[0] * a1 * 255 + xp2[0] * a2 * (255 - a1)),
								(xp1[1] * a1 * 255 + xp2[1] * a2 * (255 - a1)),
								(xp1[2] * a1 * 255 + xp2[2] * a2 * (255 - a1)),
							};

							xp2[0] = (byte)(c[0] / newA);
							xp2[1] = (byte)(c[1] / newA);
							xp2[2] = (byte)(c[2] / newA);
							xp2[3] = (byte)(newA / 255);
						}
					}
				}
			);
		}
		else
		{
			cv::remap(buf, dst, map, cv::Mat(), rsmode[shepemethod], 0);
		}

		utils->mat2yca(dst, &dt, &exdata, ow);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

int32_t moebius(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	try
	{
		bool normalize = static_cast<bool>(lua_toboolean(L, 1));
		int32_t iterX = lua_tointeger(L, 2);
		int32_t iterY = lua_tointeger(L, 3);

		std::vector<std::vector<double>> coef(4, { 0, 0 });
		auto cf = utils->GetLuaTableNum(L, "coef");

		int32_t range = cf[0][0] + 1;
		std::copy(cf.begin() + 1, cf.begin() + range, coef.begin());

		cv::Mat buf_bgra;
		auto [frame, width, height] = utils->GetPixelData(L, "");
		cv::Mat img(height, width, CV_8UC4, frame);
		if (img.empty())
		{
			std::cout << "cv::Mat array is empty..." << std::endl;
			return 0;
		}
		img.copyTo(buf_bgra);

		std::complex<double> ta(coef[0][0], coef[0][1]);
		std::complex<double> tb(coef[1][0], coef[1][1]);
		std::complex<double> tc(coef[2][0], coef[2][1]);
		std::complex<double> td(coef[3][0], coef[3][1]);

		auto ca = std::proj(ta);
		auto cb = std::proj(tb);
		auto cc = std::proj(tc);
		auto cd = std::proj(td);

		if (ca * cd == cb * cc)
			return 0;

		iterX = std::clamp(iterX, 1, 1000);
		iterY = std::clamp(iterY, 1, 1000);
		int32_t input_w = iterX * width;
		int32_t input_h = iterY * height;
		int32_t csize = input_w * input_h;
		std::vector<double> coodX(csize, 0);
		std::vector<double> coodY(csize, 0);
		Concurrency::parallel_for(0, csize, 1, [&](int32_t i)
			{
				int32_t x = i % input_w;
				int32_t y = i / input_w;

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

		cv::Mat tmp(cv::Size(input_w, input_h), buf_bgra.type());
		Concurrency::parallel_for(0, input_h, 1, [&](int32_t y)
			{
				auto pOUT = tmp.ptr<cv::Vec4b>(y);
				for (auto x = 0; x < input_w; ++x)
				{
					int32_t cdi = x + y * input_w ;
					int32_t new_x = int32_t((input_w - 1) * (coodX[cdi] - xmin) / xrange);
					int32_t new_y = int32_t((input_h - 1) * (coodY[cdi] - ymin) / yrange);

					new_x = new_x % width;
					new_y = new_y % height;
					
					auto pIN = buf_bgra.ptr<cv::Vec4b>(new_y);
					pOUT[x] = pIN[new_x];
				}
			}
		);

		cv::Mat out;
		cv::resize(tmp, out, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
		utils->PutPixelData(L, out.data);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

int32_t moebius2(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double ratio = lua_tonumber(L, 1);
	int32_t shepemethod = std::clamp(lua_tointeger(L, 2), 0, 6);

	auto efpip = utils->GetAddData("exedit.auf", 0x1b2b20);
	auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(efpip);
	auto dt = exdata->obj_edit;
	auto wt = exdata->obj_w;
	auto ht = exdata->obj_h;
	auto ow = exdata->obj_line;
	auto oh = exdata->obj_max_h;
	if ((ow < 1) || (oh < 1) || (wt < 1) || (ht < 1))
		return 0;

	cv::Mat buf(cv::Size(wt, ht), CV_8UC4);
	utils->yca2mat(&dt, buf, ow);
	if (buf.empty())
		return 0;

	try
	{
		double w2 = wt / 2.0;
		ratio = 1.0 / pow(2.0, ratio);
		double rate = ratio / w2;

		auto coef = utils->GetLuaTableNum(L, "coef");
		std::complex<double> a(coef[1][0], coef[1][1]);
		std::complex<double> b(coef[1][2], coef[1][3]);
		std::complex<double> c(coef[1][4], coef[1][5]);
		std::complex<double> d(coef[1][6], coef[1][7]);
		std::complex<double> cp(coef[1][8], coef[1][9]);

		cv::Mat map;
		map = cv::Mat(buf.size(), CV_32FC2);

		for (int32_t iy = 0; iy < ht; ++iy)
		{
			float* yp = map.ptr<float>(iy);
			double yy = iy - cp.imag();

			for (int32_t ix = 0; ix < wt; ix++)
			{
				double xx = ix - cp.real();

				std::complex<double> z(xx * rate, yy * rate);

				z = ((d * z - b) / (-c * z + a)) * w2 + cp;

				*(yp++) = (float)z.real();
				*(yp++) = (float)z.imag();
			}
		}
		cv::Mat dst(buf.size(), CV_8UC4);
		std::vector<int32_t> rsmode{
			cv::INTER_NEAREST,
			cv::INTER_LINEAR,
			cv::INTER_CUBIC,
			cv::INTER_AREA,
			cv::INTER_LANCZOS4,
			cv::INTER_LINEAR_EXACT,
			cv::INTER_NEAREST_EXACT
		};
		cv::remap(buf, dst, map, cv::Mat(), rsmode[shepemethod], 0);
		utils->mat2yca(dst, &dt, &exdata, ow);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

int32_t meanshift(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double sp = lua_tonumber(L, 1);
	double sr = lua_tonumber(L, 2);
	int32_t maxl = lua_tointeger(L, 3);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Mat tmpB, out(tmpA.size(), tmpA.type());
	cv::pyrMeanShiftFiltering(bgra[0], tmpB, sp, sr, maxl);
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t voronoi(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t pt = lua_tointeger(L, 1);
	int32_t seed = lua_tointeger(L, 2);
	bool lindrw = static_cast<bool>(lua_toboolean(L, 3));
	int32_t red = lua_tointeger(L, 4);
	int32_t green = lua_tointeger(L, 5);
	int32_t blue = lua_tointeger(L, 6);
	int32_t lint32_th = lua_tointeger(L, 7);
	int32_t lint32_tp = lua_tointeger(L, 8);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Subdiv2D subdiv;
	subdiv.initDelaunay(cv::Rect(0, 0, width, height));
	std::vector<cv::Point2f> points(pt, cv::Point2f(0.f, 0.f)), facecenter;
	std::seed_seq sq{ seed };
	std::mt19937 rn(sq);

	for (auto i = 0; i < pt; ++i)
	{
		uint32_t rnd = rn();
		float px = static_cast<float>(rnd % width);
		float py = static_cast<float>(rnd % height);
		points[i] = cv::Point2f(px, py);
	}

	points.emplace_back(cv::Point2f(0, 0));
	points.emplace_back(cv::Point2f(width - 1, 0));
	points.emplace_back(cv::Point2f(0, height - 1));
	points.emplace_back(cv::Point2f(width - 1, height - 1));
	subdiv.insert(points);

	std::vector<int32_t> idx;
	std::vector<std::vector<cv::Point2f>> facetlist;

	subdiv.getVoronoiFacetList(idx, facetlist, facecenter);
	cv::Mat vn = cv::Mat::zeros(tmpA.size(), CV_8UC3);
	std::vector<int32_t> ltype{ cv::LINE_4, cv::LINE_8, cv::LINE_AA };
	int32_t linType = ltype[lint32_tp];

	Concurrency::parallel_for_each(facetlist.begin(), facetlist.end(), [&](std::vector<cv::Point2f> ftl)
		{
			std::vector<cv::Point> vertices(ftl.size(), cv::Point());
			auto polyColor = cv::Vec3f(0.f, 0.f, 0.f);
			int32_t polyColorNum = 0;
			int32_t psize = ftl.size();

			for (int32_t i = 0; i < psize; ++i)
			{
				cv::Point vertex = ftl[i];
				vertices[i] = vertex;
				int32_t vx = vertex.x;
				int32_t vy = vertex.y;

				if (0 <= vx && vx < bgr.cols && 0 <= vy && vy < bgr.rows)
				{
					auto pIMG = bgra[0].ptr<cv::Vec3b>(vy);
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
		Concurrency::parallel_for_each(facetlist.begin(), facetlist.end(), [&](std::vector<cv::Point2f> ftl)
			{
				cv::Point2f p1 = ftl[0];
				int32_t psize = ftl.size();
				for (int32_t i = 0; i < psize; ++i)
				{
					cv::Point2f p2 = ftl[i];
					cv::line(vn, p1, p2, lincol, lint32_th, linType);
					p1 = p2;
				}
			}
		);
	}

	cv::Mat out(tmpA.size(), tmpA.type());
	bgra[0] = vn.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t delaunay(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t pt = lua_tointeger(L, 1);
	int32_t seed = lua_tointeger(L, 2);
	bool lindrw = static_cast<bool>(lua_toboolean(L, 3));
	int32_t red = lua_tointeger(L, 4);
	int32_t green = lua_tointeger(L, 5);
	int32_t blue = lua_tointeger(L, 6);
	int32_t lint32_th = lua_tointeger(L, 7);
	int32_t lint32_tp = lua_tointeger(L, 8);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Subdiv2D subdiv;
	subdiv.initDelaunay(cv::Rect(0, 0, width, height));
	std::vector<cv::Point2f> points(pt, cv::Point2f(0.f, 0.f));
	std::seed_seq sq{ seed };
	std::mt19937 rn(sq);
	for (auto i = 0; i < pt; ++i)
	{
		uint32_t rnd = rn();
		float px = static_cast<float>(rnd % width);
		float py = static_cast<float>(rnd % height);
		points[i] = cv::Point2f(px, py);
	}

	points.emplace_back(cv::Point2f(0, 0));
	points.emplace_back(cv::Point2f(width - 1, 0));
	points.emplace_back(cv::Point2f(0, height - 1));
	points.emplace_back(cv::Point2f(width - 1, height - 1));
	subdiv.insert(points);

	std::vector<cv::Vec4f> edgelist;
	std::vector<cv::Vec6f> trilist;

	subdiv.getTriangleList(trilist);
	subdiv.getEdgeList(edgelist);
	cv::Mat dn = cv::Mat::zeros(tmpA.size(), CV_8UC3);
	std::vector<int32_t> ltype{ cv::LINE_4, cv::LINE_8, cv::LINE_AA };
	int32_t linType = ltype[lint32_tp];

	Concurrency::parallel_for_each(trilist.begin(), trilist.end(), [&](cv::Vec6f tri)
		{
			std::vector<cv::Point> vertices(3, cv::Point());
			auto polyColor = cv::Vec3f(0.f, 0.f, 0.f);

			vertices[0] = cv::Point(tri[0], tri[1]);
			vertices[1] = cv::Point(tri[2], tri[3]);
			vertices[2] = cv::Point(tri[4], tri[5]);

			for (int32_t i = 0; i < 3; ++i)
			{
				auto pIMG = bgra[0].ptr<cv::Vec3b>(vertices[i].y);
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
		Concurrency::parallel_for_each(edgelist.begin(), edgelist.end(), [&dn, &lincol, lint32_th, linType](cv::Vec4f edges)
			{
				cv::line(dn, cv::Point(edges[0], edges[1]), cv::Point(edges[2], edges[3]), lincol, lint32_th, linType);
			}
		);
	}

	cv::Mat out(tmpA.size(), tmpA.type());
	bgra[0] = dn.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t pencilskc(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	float sgmS = static_cast<float>(lua_tonumber(L, 1));
	float sgmR = static_cast<float>(lua_tonumber(L, 2));
	float shade = static_cast<float>(lua_tonumber(L, 3));
	bool color = static_cast<bool>(lua_toboolean(L, 4));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat imgA(height, width, CV_8UC4, frame), tmpA;
	if (imgA.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	imgA.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	sgmS = std::clamp(sgmS, 0.f, 200.f);
	sgmR = std::clamp(sgmR, 0.f, 1.f);
	shade = std::clamp(shade, 0.f, 0.1f);

	cv::Mat tmpB, tmpC, tmpD;
	pencilSketch(bgr, tmpB, tmpC, sgmS, sgmR, shade);
	if (color)
		tmpD = tmpC.clone();
	else
		cv::cvtColor(tmpB, tmpD, cv::COLOR_GRAY2BGR);

	cv::Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpD.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t stylize(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	float sgmS = static_cast<float>(lua_tonumber(L, 1));
	float sgmR = static_cast<float>(lua_tonumber(L, 2));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	sgmS = std::clamp(sgmS, 0.f, 200.f);
	sgmR = std::clamp(sgmR, 0.f, 1.f);
	cv::Mat tmpB;
	stylization(bgr, tmpB, sgmS, sgmR);

	cv::Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t det_enhance(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	float sgmS = static_cast<float>(lua_tonumber(L, 1));
	float sgmR = static_cast<float>(lua_tonumber(L, 2));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat imgA(height, width, CV_8UC4, frame), tmpA;
	if (imgA.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	imgA.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	sgmS = std::clamp(sgmS, 0.f, 200.f);
	sgmR = std::clamp(sgmR, 0.f, 1.f);

	cv::Mat tmpB;
	detailEnhance(bgr, tmpB, sgmS, sgmR);

	cv::Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t edgePreserve(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	float sgmS = static_cast<float>(lua_tonumber(L, 1));
	float sgmR = static_cast<float>(lua_tonumber(L, 2));
	int32_t mode = lua_tointeger(L, 3);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	std::vector<int32_t> flags{
		cv::RECURS_FILTER,
		cv::NORMCONV_FILTER
	};

	sgmS = std::clamp(sgmS, 0.f, 200.f);
	sgmR = std::clamp(sgmR, 0.f, 1.f);
	cv::Mat tmpB;
	edgePreservingFilter(bgr, tmpB, flags[mode], sgmS, sgmR);

	cv::Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t oilpaint(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t size = lua_tointeger(L, 1);
	int32_t ratio = lua_tointeger(L, 2);

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat imgA(height, width, CV_8UC4, frame), tmpA;
	if (imgA.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	imgA.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	size = std::max(size, 2);
	cv::Mat tmpB;
	cv::xphoto::oilPainting(bgr, tmpB, size, ratio);

	cv::Mat out(tmpA.size(), tmpA.type());
	bgra[0] = tmpB.clone();
	cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t hist_eq(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	double limit = lua_tonumber(L, 1);
	int32_t glidW = lua_tointeger(L, 2);
	int32_t glidH = lua_tointeger(L, 3);
	bool adp = static_cast<bool>(lua_toboolean(L, 4));
	bool col = static_cast<bool>(lua_toboolean(L, 5));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat imgA(height, width, CV_8UC4, frame), tmpA;
	if (imgA.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	imgA.copyTo(tmpA);

	cv::Mat bgr(tmpA.size(), CV_8UC3), alpha(tmpA.size(), CV_8U);
	std::vector<cv::Mat> bgra{ bgr, alpha };
	std::vector<int32_t> ch{ 0,0, 1,1, 2,2, 3,3 };
	cv::mixChannels(&tmpA, 1, bgra.data(), 2, ch.data(), 4);

	cv::Mat tmpB, tmpC;
	cv::cvtColor(bgr, tmpB, cv::COLOR_BGR2GRAY);
	if (adp)
	{
		auto clahe = cv::createCLAHE(limit, cv::Size(glidW, glidH));
		clahe->apply(tmpB, tmpC);
	}
	else
	{
		cv::equalizeHist(tmpB, tmpC);
	}

	cv::Mat out(tmpA.size(), tmpA.type());
	if (col)
	{
		Concurrency::parallel_for(0, height, 1, [&](int32_t y)
			{
				auto pIN1 = tmpC.ptr<uchar>(y);
				auto pIN2 = tmpB.ptr<uchar>(y);
				auto pIN3 = tmpA.ptr<cv::Vec4b>(y);
				auto pOUT = out.ptr<cv::Vec4b>(y);

				for (auto x = 0; x < width; ++x)
				{
					double coef = 0;
					if (pIN2[x] != 0)
					{
						coef = ((double)pIN1[x]) / pIN2[x];
					}

					pOUT[x][0] = cv::saturate_cast<uchar>(coef * pIN3[x][0]);
					pOUT[x][1] = cv::saturate_cast<uchar>(coef * pIN3[x][1]);
					pOUT[x][2] = cv::saturate_cast<uchar>(coef * pIN3[x][2]);
					pOUT[x][3] = pIN3[x][3];
				}
			}
		);
	}
	else
	{
		cv::Mat tmpD;
		cv::cvtColor(tmpC, tmpD, cv::COLOR_GRAY2BGR);
		bgra[0] = tmpD.clone();
		cv::mixChannels(bgra.data(), 2, &out, 1, ch.data(), 4);
	}

	utils->PutPixelData(L, out.data);
	return 0;
}

int32_t color_map(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	int32_t cmode = lua_tointeger(L, 1);
	bool hsvmode = static_cast<bool>(lua_toboolean(L, 2));
	bool invmode = static_cast<bool>(lua_toboolean(L, 3));

	auto [frame, width, height] = utils->GetPixelData(L, "");
	cv::Mat img(height, width, CV_8UC4, frame), tmp, tmpA;
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}
	img.copyTo(tmp);

	std::vector<cv::Mat> col_ch, hsv_ch;
	cv::split(tmp, col_ch);
	cv::Mat alpha = col_ch[3].clone();

	cv::Mat tmpB, tmpC, tmpD;
	cv::cvtColor(tmp, tmpA, cv::COLOR_BGRA2BGR);
	if (hsvmode)
	{
		cv::cvtColor(tmpA, tmpB, cv::COLOR_BGR2HSV_FULL);
		cv::split(tmpB, hsv_ch);
		hsv_ch[0].convertTo(tmpC, CV_8U);
	}
	else
	{
		cv::cvtColor(tmpA, tmpC, cv::COLOR_BGR2GRAY, 1);
	}
	
	if (invmode)
	{
		cv::Mat tmp1 = cv::Mat::ones(tmpC.size(), CV_8U) * 255;
		cv::Mat tmp2 = cv::Mat::zeros(tmpC.size(), CV_8U);
		cv::subtract(tmp1, tmpC, tmp2);
		tmpC = tmp2.clone();
	}

	std::vector<int32_t> map{ cv::COLORMAP_AUTUMN,
		cv::COLORMAP_BONE,
		cv::COLORMAP_JET,
		cv::COLORMAP_WINTER,
		cv::COLORMAP_RAINBOW,
		cv::COLORMAP_OCEAN,
		cv::COLORMAP_SUMMER,
		cv::COLORMAP_SPRING,
		cv::COLORMAP_COOL,
		cv::COLORMAP_HSV,
		cv::COLORMAP_PINK,
		cv::COLORMAP_HOT,
		cv::COLORMAP_PARULA,
		cv::COLORMAP_MAGMA,
		cv::COLORMAP_INFERNO,
		cv::COLORMAP_PLASMA,
		cv::COLORMAP_VIRIDIS,
		cv::COLORMAP_CIVIDIS,
		cv::COLORMAP_TWILIGHT,
		cv::COLORMAP_TWILIGHT_SHIFTED,
		cv::COLORMAP_TURBO,
		cv::COLORMAP_DEEPGREEN
	};
	cv::applyColorMap(tmpC, tmpD, map[cmode]);

	Concurrency::parallel_for(0, img.rows, 1, [&](int32_t y)
		{
			auto pout = img.ptr<cv::Vec4b>(y);
			auto prgb = tmpD.ptr<cv::Vec3b>(y);
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
	utils->PutPixelData(L, img.data);

	return 0;
}

int32_t img_read(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	try
	{
		auto fpath = utils->toStringRestrict(L, 1);
		int32_t mode = lua_tointeger(L, 2);
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

			uint32_t w = std::max((int32_t)alpha, 0);
			uint32_t h = std::max((int32_t)beta, 0);
			uint32_t bgcol = 0x00000000;
			auto bitmap = document->renderToBitmap(w, h, bgcol);
			bitmap.convertToRGBA();
			auto fdata = reinterpret_cast<void*>(bitmap.data());
			cv::Mat f(cv::Size(bitmap.width(), bitmap.height()), CV_8UC4, fdata);
			if (f.empty())
			{
				std::cout << "cv::Mat array is empty..." << std::endl;
				return 0;
			}
			cv::cvtColor(f, img, cv::COLOR_BGRA2RGBA);
		}

		img_read_tmp(fpath, mode, img);
		if (img.empty())
		{
			std::cout << "cv::Mat array is empty..." << std::endl;
			return 0;
		}

		auto dp = utils->GetAddData("exedit.auf", 0x1b2b20);
		auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(dp);
		auto ow = exdata->obj_line;
		auto oh = exdata->obj_max_h;
		if (oh < img.rows || (ow - 8) < img.cols)
		{
			std::cout << "cv::Mat array is too bigs..." << std::endl;
			return 0;
		}

		if (img.channels() == 3)
		{
			cv::cvtColor(img, tmp, cv::COLOR_BGR2BGRA);
		}
		else if (img.channels() == 1)
		{
			cv::cvtColor(img, tmp, cv::COLOR_GRAY2BGRA);
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

		utils->PutPixelData(L, frame.data);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

int32_t img_call(lua_State* L)
{
	cv::Mat img;
	img_read_tmp("", -1, img);
	if (img.empty())
	{
		std::cout << "cv::Mat array is empty..." << std::endl;
		return 0;
	}

	int32_t w = img.cols;
	int32_t h = img.rows;
	lua_pushlightuserdata(L, img.data);
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);

	return 3;
}

int32_t mov_read(lua_State* L)
{
	std::unique_ptr<AviUtl::beRL::Util> utils(new AviUtl::beRL::Util());
	try
	{
		int32_t idx = lua_tointeger(L, 1);
		double alpha = lua_tonumber(L, 2);
		double beta = lua_tonumber(L, 3);
		int32_t midx = lua_tointeger(L, 4);
		bool reload = static_cast<bool>(lua_toboolean(L, 5));

		auto dp = utils->GetAddData("exedit.auf", 0x1b2b20);
		auto exdata = reinterpret_cast<ExEdit::FilterProcInfo*>(dp);
		auto tn = exdata->frame_num;
		auto ow = exdata->obj_line;
		auto oh = exdata->obj_max_h;

		std::vector<int32_t> mode{
			cv::CAP_ANY,
			cv::CAP_FFMPEG,
			cv::CAP_IMAGES
		};

		static std::string path("");
		static std::unique_ptr<cv::VideoCapture> vcp;
		static double fnum = 0;
		static int32_t fnum_idx = 0;
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
				fnum_idx = static_cast<int32_t>(fnum);
			}
			else
			{
				return 0;
			}
		}

		if (idx == -2)
		{
			idx = tn % fnum_idx;
		}
		else if (idx == -1)
		{
			idx = std::clamp(tn, 0, fnum_idx);
		}
		else
		{
			idx = std::clamp(idx, 0, fnum_idx);
		}
		double val = static_cast<double>(idx);

		bool isset = vcp->set(cv::CAP_PROP_POS_FRAMES, val);
		if (!isset)
			return 0;

		cv::Mat frame, img, tmp;
		bool isread = vcp->read(img);
		if ((!isread) || img.empty())
		{
			std::cout << "cv::Mat array is empty..." << std::endl;
			return 0;
		}

		if (oh < img.rows || (ow - 8) < img.cols)
		{
			std::cout << "cv::Mat array is too bigs..." << std::endl;
			return 0;
		}

		if (img.channels() == 3)
		{
			cv::cvtColor(img, tmp, cv::COLOR_BGR2BGRA);
		}
		else if (img.channels() == 1)
		{
			cv::cvtColor(img, tmp, cv::COLOR_GRAY2BGRA);
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

		utils->PutPixelData(L, frame.data);
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

/********************************************************************************/
/*[utility functions]*/

static inline double modulo(double x, double y)
{
	double r = fmod(x, y);
	if (r < 0) {
		r += y;
	}
	return r;
}

void img_read_tmp(cv::String path, int32_t idx, cv::Mat& dst)
{
	static cv::Mat img;
	std::vector<int32_t> mode{
		cv::IMREAD_UNCHANGED,
		cv::IMREAD_COLOR | cv::IMREAD_ANYDEPTH,
		cv::IMREAD_GRAYSCALE | cv::IMREAD_ANYDEPTH,
		cv::IMREAD_REDUCED_COLOR_2,
		cv::IMREAD_REDUCED_COLOR_4,
		cv::IMREAD_REDUCED_COLOR_8
	};

	if (0 <= idx)
	{
		img = cv::imread(path, mode[idx]);
	}
	else if (idx == -2)
	{
		img = dst.clone();
		return;
	}
	dst = img.clone();
}
