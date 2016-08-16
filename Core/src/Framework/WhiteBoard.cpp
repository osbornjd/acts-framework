#include "ACTFW/Framework/WhiteBoard.hpp"

FW::WhiteBoard::WhiteBoard(std::unique_ptr<Acts::Logger> logger)
  : m_logger(std::move(logger))
{
}

FW::WhiteBoard::~WhiteBoard()
{
}
