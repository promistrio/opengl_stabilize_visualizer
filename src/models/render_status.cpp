#include "render_status.hpp"
bool RenderStatus::canUseTexture()
{
    return _canUseTexture;
}

bool RenderStatus::canPrepareTexture()
{
    return _canPrepareTexture;
}

void RenderStatus::textureWasUsed()
{
    _mutex.lock();
    _canUseTexture = false;
    _canPrepareTexture = true;
    _mutex.unlock();
}

void RenderStatus::textureWasPrepared()
{
    _mutex.lock();
    _canUseTexture = true;
    _canPrepareTexture = false;
    _mutex.unlock();
}

unsigned char * RenderStatus::getTexturePointer()
{
    return _bgrtest.data;
}
cv::Mat RenderStatus::getTextureMat()
{
    return _bgrtest;
}