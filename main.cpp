/*
 * Read video frame with FFmpeg and convert to OpenCV image
 *
 * Copyright (c) 2016 yohhoy
 */
#include <iostream>
#include <vector>
// FFmpeg
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
}
// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


#include <iostream>

#include "ffmpegcpp.h"

using namespace std;
using namespace ffmpegcpp;

class PGMFileSink : public VideoFrameSink, public FrameWriter
{
public:

	PGMFileSink()
	{
	}

	FrameSinkStream* CreateStream()
	{
		stream = new FrameSinkStream(this, 0);
		return stream;
	}

	virtual void WriteFrame(int /* streamIndex */, AVFrame* frame, StreamData*  /* streamData */)
	{
		++frameNumber;
		printf("saving frame %3d\n", frameNumber);
		fflush(stdout);


		// write the first channel's color data to a PGM file.
		// This raw image file can be opened with most image editing programs.
		snprintf(fileNameBuffer, sizeof(fileNameBuffer), "frames/pgm-%d.pgm", frameNumber);
		//pgm_save(frame->data[0], frame->linesize[0],
		//	frame->width, frame->height, fileNameBuffer);

        SwsContext* swsctx = sws_getCachedContext(
        nullptr, frame->width, frame->height, AV_PIX_FMT_BGR32,
        frame->width, frame->height, AV_PIX_FMT_GRAY8, SWS_BICUBIC, nullptr, nullptr, nullptr);

        if (!swsctx) {
            std::cerr << "fail to sws_getCachedContext";
            exit(-1);
        }

        //sws_scale(swsctx, frame->data, frame->linesize, 0, frame->height, frame->data, frame->linesize);

        cv::Mat image(frame->height, frame->width, CV_8UC1, frame->data[0],0);
        cv::imshow("press ESC to exit", image);
        if (cv::waitKey(1) == 0x1b){
            exit(0);
        }
	}

	void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
		char *filename)
	{
		FILE *f;
		int i;

		f = fopen(filename, "w");
		fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
		for (i = 0; i < ysize; i++)
			fwrite(buf + i * wrap, 1, xsize, f);
		fclose(f);
	}

	virtual void Close(int /* streamIndex */)
	{
		delete stream;
	}

	virtual bool IsPrimed()
	{
		// Return whether we have all information we need to start writing out data.
		// Since we don't really need any data in this use case, we are always ready.
		// A container might only be primed once it received at least one frame from each source
		// it will be muxing together (see Muxer.cpp for how this would work then).
		return true;
	}

private:
	char fileNameBuffer[1024];
	int frameNumber = 0;
	FrameSinkStream* stream;

};

int main()
{
	// This example will decode a video stream from a container and output it as raw image data, one image per frame.
	try
	{
		// Load this container file so we can extract video from it.
		Demuxer* demuxer = new Demuxer("/media/user/Common/DEV/ffmpeg_opencv/build/dump.bin");//../../samples/big_buck_bunny.mp4

		// Create a file sink that will just output the raw frame data in one PGM file per frame.
		PGMFileSink* fileSink = new PGMFileSink();

		// tie the file sink to the best video stream in the input container.
		demuxer->DecodeBestVideoStream(fileSink);

		// Prepare the output pipeline. This will push a small amount of frames to the file sink until it IsPrimed returns true.
		demuxer->PreparePipeline();

		// Push all the remaining frames through.
		while (!demuxer->IsDone())
		{
			demuxer->Step();
		}

		// done
		delete demuxer;
		delete fileSink;
	}
	catch (FFmpegException e)
	{
		cerr << "Exception caught!" << endl;
		throw e;
	}

	cout << "Decoding complete!" << endl;
}
/*
int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: ff2cv <infile>" << std::endl;
        return 1;
    }
    const char* infile = argv[1];

    // initialize FFmpeg library
    // av_log_set_level(AV_LOG_DEBUG);
    int ret;

    // open input file context
    AVFormatContext* inctx = nullptr;
    ret = avformat_open_input(&inctx, infile, nullptr, nullptr);
    if (ret < 0) {
        std::cerr << "fail to avforamt_open_input(\"" << infile << "\"): ret=" << ret;
        return 2;
    }
    // retrive input stream information
    ret = avformat_find_stream_info(inctx, nullptr);
    if (ret < 0) {
        std::cerr << "fail to avformat_find_stream_info: ret=" << ret;
        return 2;
    }

    // find primary video stream
    AVCodec* vcodec = nullptr;
    ret = av_find_best_stream(inctx, AVMEDIA_TYPE_VIDEO, -1, -1, &vcodec, 0);
    if (ret < 0) {
        std::cerr << "fail to av_find_best_stream: ret=" << ret;
        return 2;
    }
    const int vstrm_idx = ret;
    AVStream* vstrm = inctx->streams[vstrm_idx];

    // open video decoder context
    ret = avcodec_open2(vstrm->codec, vcodec, nullptr);
    if (ret < 0) {
        std::cerr << "fail to avcodec_open2: ret=" << ret;
        return 2;
    }

    // print input video stream informataion
    std::cout
        << "infile: " << infile << "\n"
        << "format: " << inctx->iformat->name << "\n"
        << "vcodec: " << vcodec->name << "\n"
        << "size:   " << vstrm->codec->width << 'x' << vstrm->codec->height << "\n"
        << "fps:    " << av_q2d(vstrm->codec->framerate) << " [fps]\n"
        << "length: " << av_rescale_q(vstrm->duration, vstrm->time_base, {1,1000}) / 1000. << " [sec]\n"
        << "pixfmt: " << av_get_pix_fmt_name(vstrm->codec->pix_fmt) << "\n"
        << "frame:  " << vstrm->nb_frames << "\n"
        << std::flush;

    // initialize sample scaler
    const int dst_width = vstrm->codec->width;
    const int dst_height = vstrm->codec->height;
    const AVPixelFormat dst_pix_fmt = AV_PIX_FMT_BGR24;
    SwsContext* swsctx = sws_getCachedContext(
        nullptr, vstrm->codec->width, vstrm->codec->height, vstrm->codec->pix_fmt,
        dst_width, dst_height, dst_pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
    if (!swsctx) {
        std::cerr << "fail to sws_getCachedContext";
        return 2;
    }
    std::cout << "output: " << dst_width << 'x' << dst_height << ',' << av_get_pix_fmt_name(dst_pix_fmt) << std::endl;

    // allocate frame buffer for output
    AVFrame* frame = av_frame_alloc();
    std::vector<uint8_t> framebuf(avpicture_get_size(dst_pix_fmt, dst_width, dst_height));
    avpicture_fill(reinterpret_cast<AVPicture*>(frame), framebuf.data(), dst_pix_fmt, dst_width, dst_height);

    // decoding loop
    AVFrame* decframe = av_frame_alloc();
    unsigned nb_frames = 0;
    bool end_of_stream = false;
    int got_pic = 0;
    AVPacket pkt;
    do {
        if (!end_of_stream) {
            // read packet from input file
            ret = av_read_frame(inctx, &pkt);
            if (ret < 0 && ret != AVERROR_EOF) {
                std::cerr << "fail to av_read_frame: ret=" << ret;
                return 2;
            }
            if (ret == 0 && pkt.stream_index != vstrm_idx)
                goto next_packet;
            end_of_stream = (ret == AVERROR_EOF);
        }
        if (end_of_stream) {
            // null packet for bumping process
            av_init_packet(&pkt);
            pkt.data = nullptr;
            pkt.size = 0;
        }
        // decode video frame
        avcodec_decode_video2(vstrm->codec, decframe, &got_pic, &pkt);
        if (!got_pic)
            goto next_packet;
        // convert frame to OpenCV matrix
        sws_scale(swsctx, decframe->data, decframe->linesize, 0, decframe->height, frame->data, frame->linesize);
        {
        cv::Mat image(dst_height, dst_width, CV_8UC3, framebuf.data(), frame->linesize[0]);
        cv::imshow("press ESC to exit", image);
        if (cv::waitKey(1) == 0x1b)
            break;
        }
        std::cout << nb_frames << '\r' << std::flush;  // dump progress
        ++nb_frames;
next_packet:
        av_free_packet(&pkt);
    } while (!end_of_stream || got_pic);
    std::cout << nb_frames << " frames decoded" << std::endl;

    av_frame_free(&decframe);
    av_frame_free(&frame);
    avcodec_close(vstrm->codec);
    avformat_close_input(&inctx);
    return 0;
}*/