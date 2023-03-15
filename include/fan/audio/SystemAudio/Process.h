TH_mutex_t PlayInfoListMutex;
_PlayInfoList_t PlayInfoList;
SoundPlayUnique_t PlayInfoListUnique = 0;

uint32_t GroupAmount = 0;
struct _Group_t {
  _PlayInfoList_NodeReference_t FirstReference;
  _PlayInfoList_NodeReference_t LastReference;
}*GroupList = nullptr;

struct _Play_t {
  _PlayInfoList_NodeReference_t Reference;
};
VEC_t PlayList;

TH_mutex_t MessageQueueListMutex;
VEC_t MessageQueueList;

_DecoderList_t DecoderList[_constants::Opus::SupportedChannels];

_CacheList_t CacheList;

sint32_t Open() {
  TH_mutex_init(&this->PlayInfoListMutex);
  this->PlayInfoList.Open();

  VEC_init(&this->PlayList, sizeof(_Play_t), A_resize);

  TH_mutex_init(&this->MessageQueueListMutex);
  VEC_init(&this->MessageQueueList, sizeof(_Message_t), A_resize);

  for(uint32_t Channel = 0; Channel < _constants::Opus::SupportedChannels; Channel++){
    uint32_t size = sizeof(_DecoderHead_t);
    size += opus_decoder_get_size(Channel + 1);
    _DecoderList_t *dl = &this->DecoderList[Channel];
    dl->Open(size);
    for(uint32_t i = 0; i < _constants::Opus::CacheDecoderPerChannel; i++){
      _DecoderList_NodeReference_t r = dl->NewNodeLast_alloc();
      uint8_t *dld = (uint8_t *)(*dl)[r];

      _DecoderHead_t *DecoderHead = (_DecoderHead_t *)dld;
      DecoderHead->CacheID = _CacheList_gnric();

      OpusDecoder *od = (OpusDecoder *)&dld[sizeof(_DecoderHead_t)];
      int re = opus_decoder_init(od, 48000, Channel + 1);
      if(re != OPUS_OK){
        fan::throw_error("opus_decoder_init");
      }
    }
  }

  this->CacheList.Open();
  for(uint32_t i = 0; i < _constants::Opus::CacheSegmentAmount; i++){
    _CacheList_NodeReference_t r = this->CacheList.NewNodeLast_alloc();
    auto Data = &this->CacheList[r];
    Data->SegmentID = (_SegmentID_t)-1;
    Data->DecoderID = _DecoderList_gnric();
  }

  return 0;
}
void Close() {
  this->PlayInfoList.Close();
  A_resize(this->GroupList, 0);
  VEC_free(&this->PlayList);
  VEC_free(&this->MessageQueueList);
}

void _DecodeCopy(
  uint8_t ChannelAmount,
  f32_t *F2_20,
  f32_t *Output
){
  switch(ChannelAmount){
    case 1:{
      /* need manuel interleave */
      for(uint32_t i = 0; i < _constants::Opus::SegmentFrameAmount20; i++){
        Output[i * 2 + 0] = F2_20[i];
        Output[i * 2 + 1] = F2_20[i];
      }
      break;
    }
    case 2:{
      MEM_copy(
        F2_20,
        Output,
        _constants::Opus::SegmentFrameAmount20 * 2 * sizeof(f32_t));
      break;
    }
  }
}

_CacheID_t _GetCacheID(piece_t *piece, _SegmentID_t SegmentID){
  _CacheID_t clid = this->CacheList.GetNodeLast();
  auto Cache = &this->CacheList[clid];
  if(Cache->SegmentID != (_SegmentID_t)-1){
    Cache->piece->SACSegment[Cache->SegmentID].CacheID = _CacheList_gnric();
    _DecoderID_t dlid = Cache->DecoderID;
    if(_DecoderList_inric(dlid) == false){
      _DecoderList_t *dl = &this->DecoderList[Cache->piece->ChannelAmount - 1];
      _DecoderHead_t *DecoderHead = (_DecoderHead_t *)(*dl)[dlid];
      DecoderHead->CacheID = _CacheList_gnric();
      dl->ReLinkAsLast(dlid);
    }
  }
  Cache->piece = piece;
  Cache->SegmentID = SegmentID;
  piece->SACSegment[SegmentID].CacheID = clid;
  return clid;
}

void _WarmUpDecoder(piece_t *piece, _SegmentID_t SegmentID, OpusDecoder *od){
  opus_decoder_ctl(od, OPUS_RESET_STATE);

  _SegmentID_t fsid = SegmentID - _constants::Opus::DecoderWarmUpAmount;
  if(SegmentID < _constants::Opus::DecoderWarmUpAmount){
    fsid = 0;
  }

  while(fsid != SegmentID){
    auto SACSegment = &piece->SACSegment[fsid];

    f32_t F2_20[_constants::Opus::SegmentFrameAmount20 * 5 * 2];
    sint32_t oerr = opus_decode_float(
      od,
      &piece->SACData[SACSegment->Offset],
      SACSegment->Size,
      F2_20,
      _constants::Opus::SegmentFrameAmount20 * 5,
      0);

    if(oerr != _constants::Opus::SegmentFrameAmount20){
      fan::print("help", oerr);
      fan::throw_error("a");
    }

    fsid++;
  }
}

_DecoderID_t _GetDecoderID(piece_t *piece, _SegmentID_t SegmentID){
  _DecoderList_t *DecoderList = &this->DecoderList[piece->ChannelAmount - 1];

  if(SegmentID != 0){
    auto pCacheID = piece->SACSegment[SegmentID - 1].CacheID;
    if(_CacheList_inric(pCacheID) == false){
      auto pCache = &this->CacheList[pCacheID];
      _DecoderID_t DecoderID = pCache->DecoderID;
      if(_DecoderList_inric(DecoderID) == false){
        pCache->DecoderID = _DecoderList_gnric();
        return DecoderID;
      }
    }
  }

  _DecoderID_t DecoderID = DecoderList->GetNodeLast();

  _DecoderHead_t *DecoderHead = (_DecoderHead_t *)(*DecoderList)[DecoderID];

  _CacheID_t dclid = DecoderHead->CacheID;
  if(_CacheList_inric(dclid) == false){
    auto dCache = &this->CacheList[dclid];
    dCache->DecoderID = _DecoderList_gnric();
  }

  OpusDecoder *od = (OpusDecoder *)&DecoderHead[1];

  this->_WarmUpDecoder(piece, SegmentID, od);

  return DecoderID;
}

void
_LinkDecoderCache(
  _DecoderList_t *DecoderList,
  _DecoderID_t DecoderID,
  _CacheID_t CacheID
){
  _DecoderHead_t *DecoderHead = (_DecoderHead_t *)(*DecoderList)[DecoderID];
  DecoderHead->CacheID = CacheID;
  auto Cache = &this->CacheList[CacheID];
  Cache->DecoderID = DecoderID;
}

void _DecodeSegment(piece_t *piece, _SegmentID_t SegmentID){
  _DecoderList_t *DecoderList = &this->DecoderList[piece->ChannelAmount - 1];

  _DecoderID_t DecoderID = this->_GetDecoderID(piece, SegmentID);
  DecoderList->ReLinkAsFirst(DecoderID);

  _CacheID_t CacheID = this->_GetCacheID(piece, SegmentID);

  this->_LinkDecoderCache(DecoderList, DecoderID, CacheID);

  OpusDecoder *od = (OpusDecoder *)&((_DecoderHead_t *)(*DecoderList)[DecoderID])[1];

  auto SACSegment = &piece->SACSegment[SegmentID];

  f32_t F2_20[_constants::Opus::SegmentFrameAmount20 * 5 * 2];
  sint32_t oerr = opus_decode_float(
    od,
    &piece->SACData[SACSegment->Offset],
    SACSegment->Size,
    F2_20,
    _constants::Opus::SegmentFrameAmount20 * 5,
    0);

  if(oerr != _constants::Opus::SegmentFrameAmount20){
    fan::print("help", oerr);
    fan::throw_error("a");
  }

  auto Cache = &this->CacheList[CacheID];
  _DecodeCopy(piece->ChannelAmount, F2_20, Cache->Samples);
}

void _GetFrames(piece_t *piece, uint64_t Offset, f32_t **FramePointer, uint32_t *FrameAmount) {
  Offset += piece->BeginCut;
  _SegmentID_t SegmentID = Offset / _constants::FrameCacheAmount;
  uint32_t PieceCacheMod = Offset % _constants::FrameCacheAmount;
  auto CacheID = &piece->SACSegment[SegmentID].CacheID;
  if(_CacheList_inric(*CacheID) == true){
    _DecodeSegment(piece, SegmentID);
  }
  this->CacheList.ReLinkAsFirst(*CacheID);
  auto Cache = &this->CacheList[*CacheID];
  *FramePointer = &Cache->Samples[PieceCacheMod * _constants::ChannelAmount];
  *FrameAmount = _constants::FrameCacheAmount - PieceCacheMod;
}

void _AddSoundToPlay(_PlayInfoList_NodeReference_t PlayInfoReference) {
  VEC_handle0(&this->PlayList, 1);
  uint32_t PlayID = this->PlayList.Current - 1;
  _Play_t *Play = &((_Play_t *)this->PlayList.ptr)[PlayID];
  Play->Reference = PlayInfoReference;
  auto PlayInfo = &this->PlayInfoList[PlayInfoReference];
  #if fan_debug >= 0
    if (PlayInfo->PlayID != (uint32_t)-1) {
      /* trying play sound that already playing */
      fan::throw_error("fan_debug");
    }
  #endif
  PlayInfo->PlayID = PlayID;
}
void
_RemoveFromPlayList
(
  uint32_t PlayID
){
  /* super fast remove */
  ((_Play_t *)this->PlayList.ptr)[PlayID] = ((_Play_t *)this->PlayList.ptr)[--this->PlayList.Current];

  /* moved one needs update */
  _PlayInfoList_NodeReference_t PlayInfoReference = ((_Play_t *)this->PlayList.ptr)[PlayID].Reference;
  auto PlayInfo = &this->PlayInfoList[PlayInfoReference];
  PlayInfo->PlayID = PlayID;
}
void
_RemoveFromPlayInfoList
(
  _PlayInfoList_NodeReference_t PlayInfoReference,
  const PropertiesSoundStop_t *Properties
){
  auto PlayInfo = &this->PlayInfoList[PlayInfoReference];
  if (PlayInfo->PlayID == (uint32_t)-1) {
    /* properties are ignored */
    TH_lock(&this->PlayInfoListMutex);
    this->PlayInfoList.unlrec(PlayInfoReference);
    TH_unlock(&this->PlayInfoListMutex);
  }
  else {
    auto PlayInfo = &this->PlayInfoList[PlayInfoReference];
    if (Properties->FadeOutTo != 0) {
      PropertiesSoundPlay_t* PropertiesPlay = &PlayInfo->properties;
      if (PropertiesPlay->Flags.FadeIn) {
        PropertiesPlay->Flags.FadeIn = false;
        PropertiesPlay->Flags.FadeOut = true;
        f32_t CurrentVolume = PropertiesPlay->FadeFrom / PropertiesPlay->FadeTo;
        PropertiesPlay->FadeTo = Properties->FadeOutTo;
        PropertiesPlay->FadeFrom = ((f32_t)1 - CurrentVolume) * PropertiesPlay->FadeTo;
      }
      else {
        PropertiesPlay->FadeFrom = 0;
        PropertiesPlay->FadeTo = Properties->FadeOutTo;
        PropertiesPlay->Flags.FadeOut = true;
      }
    }
    else {
      this->_RemoveFromPlayList(PlayInfo->PlayID);
      TH_lock(&this->PlayInfoListMutex);
      this->PlayInfoList.unlrec(PlayInfoReference);
      TH_unlock(&this->PlayInfoListMutex);
    }
  }
}

void _DataCallback(f32_t *Output) {
  if (this->MessageQueueList.Current) {
    TH_lock(&this->MessageQueueListMutex);
    for (uint32_t i = 0; i < this->MessageQueueList.Current; i++) {
      _Message_t* Message = &((_Message_t*)this->MessageQueueList.ptr)[i];
      switch (Message->Type) {
        case _MessageType_t::SoundPlay: {
          this->_AddSoundToPlay(Message->Data.SoundPlay.PlayInfoReference);
          break;
        }
        case _MessageType_t::SoundStop: {
          auto &SoundPlayID = Message->Data.SoundStop.SoundPlayID;
          auto &nr = SoundPlayID.nr;
          if(PlayInfoList.inri(nr) == true){
            break;
          }
          if(PlayInfoList.IsNRSentienel(nr) == true){
            break;
          }
          if(PlayInfoList.IsNodeReferenceRecycled(nr) == true){
            break;
          }
          auto &node = PlayInfoList[nr];
          if(node.unique != SoundPlayID.unique){
            break;
          }
          this->_RemoveFromPlayInfoList(nr, &Message->Data.SoundStop.Properties);
          break;
        }
        case _MessageType_t::PauseGroup: {
          uint32_t GroupID = Message->Data.PauseGroup.GroupID;
          TH_lock(&this->PlayInfoListMutex);
          _PlayInfoList_NodeReference_t LastPlayInfoReference = this->GroupList[GroupID].LastReference;
          _PlayInfoList_NodeReference_t PlayInfoReference = this->GroupList[GroupID].FirstReference;
          PlayInfoReference = PlayInfoReference.Next(&this->PlayInfoList);
          TH_unlock(&this->PlayInfoListMutex);
          while (PlayInfoReference != LastPlayInfoReference) {
            auto PlayInfoNode = this->PlayInfoList.GetNodeByReference(PlayInfoReference);
            if (PlayInfoNode->data.PlayID != (uint32_t)-1) {
              this->_RemoveFromPlayList(PlayInfoNode->data.PlayID);
              PlayInfoNode->data.PlayID = (uint32_t)-1;
            }
            PlayInfoReference = PlayInfoNode->NextNodeReference;
          }
          break;
        }
        case _MessageType_t::ResumeGroup: {
          uint32_t GroupID = Message->Data.PauseGroup.GroupID;
          TH_lock(&this->PlayInfoListMutex);
          _PlayInfoList_NodeReference_t LastPlayInfoReference = this->GroupList[GroupID].LastReference;
          _PlayInfoList_NodeReference_t PlayInfoReference = this->GroupList[GroupID].FirstReference;
          PlayInfoReference = PlayInfoReference.Next(&this->PlayInfoList);
          TH_unlock(&this->PlayInfoListMutex);
          while (PlayInfoReference != LastPlayInfoReference) {
            auto PlayInfoNode = this->PlayInfoList.GetNodeByReference(PlayInfoReference);
            if (PlayInfoNode->data.PlayID == (uint32_t)-1) {
              this->_AddSoundToPlay(PlayInfoReference);
            }
            PlayInfoReference = PlayInfoNode->NextNodeReference;
          }
          break;
        }
        case _MessageType_t::StopGroup: {
          uint32_t GroupID = Message->Data.PauseGroup.GroupID;
          TH_lock(&this->PlayInfoListMutex);
          _PlayInfoList_NodeReference_t LastPlayInfoReference = this->GroupList[GroupID].LastReference;
          _PlayInfoList_NodeReference_t PlayInfoReference = this->GroupList[GroupID].FirstReference;
          PlayInfoReference = PlayInfoReference.Next(&this->PlayInfoList);
          while (PlayInfoReference != LastPlayInfoReference) {
            auto PlayInfoNode = this->PlayInfoList.GetNodeByReference(PlayInfoReference);
            PropertiesSoundStop_t Properties;
            this->_RemoveFromPlayInfoList(PlayInfoReference, &Properties);
            PlayInfoReference = PlayInfoNode->NextNodeReference;
          }
          TH_unlock(&this->PlayInfoListMutex);
        }
      }
    }
    this->MessageQueueList.Current = 0;
    TH_unlock(&this->MessageQueueListMutex);
  }
  for (uint32_t PlayID = 0; PlayID < this->PlayList.Current;) {
    _Play_t *Play = &((_Play_t *)this->PlayList.ptr)[PlayID];
    _PlayInfoList_NodeReference_t PlayInfoReference = Play->Reference;
    auto PlayInfo = &this->PlayInfoList[PlayInfoReference];
    piece_t *piece = PlayInfo->piece;
    PropertiesSoundPlay_t *Properties = &PlayInfo->properties;
    uint32_t OutputIndex = 0;
    uint32_t CanBeReadFrameCount;
    struct {
      f32_t FadePerFrame;
    }CalculatedVariables;
    gt_ReOffset:
    CanBeReadFrameCount = piece->GetFrameAmount() - PlayInfo->offset;
    if (CanBeReadFrameCount > _constants::CallFrameCount - OutputIndex) {
      CanBeReadFrameCount = _constants::CallFrameCount - OutputIndex;
    }
    if (Properties->Flags.FadeIn || Properties->Flags.FadeOut) {
      f32_t TotalFade = Properties->FadeTo - Properties->FadeFrom;
      f32_t CurrentFadeTime = (f32_t)CanBeReadFrameCount / _constants::opus_decode_sample_rate;
      if (TotalFade < CurrentFadeTime) {
        CanBeReadFrameCount = TotalFade * _constants::opus_decode_sample_rate;
        if (CanBeReadFrameCount == 0) {
          if(Properties->Flags.FadeIn == true){
            Properties->Flags.FadeIn = false;
          }
          else if(Properties->Flags.FadeOut == true){
            PropertiesSoundStop_t PropertiesSoundStop;
            this->_RemoveFromPlayInfoList(PlayInfoReference, &PropertiesSoundStop);
            continue;
          }
        }
      }
      CalculatedVariables.FadePerFrame = (f32_t)1 / (Properties->FadeTo * _constants::opus_decode_sample_rate);
    }
    while (CanBeReadFrameCount != 0) {
      f32_t* FrameCachePointer;
      uint32_t FrameCacheAmount;
      this->_GetFrames(
        piece,
        PlayInfo->offset,
        &FrameCachePointer,
        &FrameCacheAmount);
      if (FrameCacheAmount > CanBeReadFrameCount) {
        FrameCacheAmount = CanBeReadFrameCount;
      }
      if (Properties->Flags.FadeIn) {
        f32_t CurrentVolume = Properties->FadeFrom / Properties->FadeTo;
        for (uint32_t i = 0; i < FrameCacheAmount; i++) {
          for (uint32_t iChannel = 0; iChannel < _constants::ChannelAmount; iChannel++) {
            ((f32_t*)Output)[(OutputIndex + i) * _constants::ChannelAmount + iChannel] += FrameCachePointer[i * _constants::ChannelAmount + iChannel] * CurrentVolume;
          }
          CurrentVolume += CalculatedVariables.FadePerFrame;
        }
        Properties->FadeFrom += (f32_t)FrameCacheAmount / _constants::opus_decode_sample_rate;
      }
      else if (Properties->Flags.FadeOut) {
        f32_t CurrentVolume = Properties->FadeFrom / Properties->FadeTo;
        CurrentVolume = (f32_t)1 - CurrentVolume;
        for (uint32_t i = 0; i < FrameCacheAmount; i++) {
          for (uint32_t iChannel = 0; iChannel < _constants::ChannelAmount; iChannel++) {
            ((f32_t*)Output)[(OutputIndex + i) * _constants::ChannelAmount + iChannel] += FrameCachePointer[i * _constants::ChannelAmount + iChannel] * CurrentVolume;
          }
          CurrentVolume -= CalculatedVariables.FadePerFrame;
        }
        Properties->FadeFrom += (f32_t)FrameCacheAmount / _constants::opus_decode_sample_rate;
      }
      else {
        std::transform(
          &FrameCachePointer[0],
          &FrameCachePointer[FrameCacheAmount * _constants::ChannelAmount],
          &((f32_t*)Output)[OutputIndex * _constants::ChannelAmount],
          &((f32_t*)Output)[OutputIndex * _constants::ChannelAmount],
          std::plus<f32_t>{});
      }
      PlayInfo->offset += FrameCacheAmount;
      OutputIndex += FrameCacheAmount;
      CanBeReadFrameCount -= FrameCacheAmount;
    }

    #define JumpIfNeeded() \
      if(OutputIndex != _constants::CallFrameCount) { goto gt_ReOffset; }

    if (PlayInfo->offset == piece->GetFrameAmount()) {
      if (Properties->Flags.Loop == true) {
        PlayInfo->offset = 0;
      }
    }
    if (Properties->Flags.FadeIn) {
      if (Properties->FadeFrom >= Properties->FadeTo) {
        Properties->Flags.FadeIn = false;
      }
      JumpIfNeeded();
    }
    else if (Properties->Flags.FadeOut) {
      if (Properties->FadeFrom >= Properties->FadeTo) {
        PropertiesSoundStop_t PropertiesSoundStop;
        this->_RemoveFromPlayInfoList(PlayInfoReference, &PropertiesSoundStop);
        continue;
      }
      JumpIfNeeded();
    }
    else{
      if (PlayInfo->offset == piece->GetFrameAmount()) {
        PropertiesSoundStop_t PropertiesSoundStop;
        this->_RemoveFromPlayInfoList(PlayInfoReference, &PropertiesSoundStop);
        continue;
      }
      JumpIfNeeded();
    }

    #undef JumpIfNeeded

    PlayID++;
  }
}
