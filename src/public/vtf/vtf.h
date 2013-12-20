//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef VTF_H
#define VTF_H

#ifdef _WIN32
#pragma once
#endif

#include "bitmap/imageformat.h"

// #define XTF_FILE_FORMAT_ONLY to just include the vtf / xtf structures, and none of the code declaration
#ifndef XTF_FILE_FORMAT_ONLY

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CUtlBuffer;
class Vector;
struct Rect_t;


//-----------------------------------------------------------------------------
// Texture flags
//-----------------------------------------------------------------------------
enum
{
	// flags from the *.txt config file
	TEXTUREFLAGS_POINTSAMPLE	               = 0x00000001,
	TEXTUREFLAGS_TRILINEAR		               = 0x00000002,
	TEXTUREFLAGS_CLAMPS			               = 0x00000004,
	TEXTUREFLAGS_CLAMPT			               = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC	               = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5		               = 0x00000020,
	TEXTUREFLAGS_NOCOMPRESS		               = 0x00000040,
	TEXTUREFLAGS_NORMAL			               = 0x00000080,
	TEXTUREFLAGS_NOMIP			               = 0x00000100,
	TEXTUREFLAGS_NOLOD			               = 0x00000200,
	TEXTUREFLAGS_MINMIP			               = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL		               = 0x00000800,
	
	// These are automatically generated by vtex from the texture data.
	TEXTUREFLAGS_ONEBITALPHA	               = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA	               = 0x00002000,

	// newer flags from the *.txt config file
	TEXTUREFLAGS_ENVMAP			               = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET	               = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET	           = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE               = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY		               = 0x00040000,
	TEXTUREFLAGS_ONEOVERMIPLEVELINALPHA        = 0x00080000,
	TEXTUREFLAGS_PREMULTCOLORBYONEOVERMIPLEVEL = 0x00100000,
	TEXTUREFLAGS_NORMALTODUDV                  = 0x00200000,
	TEXTUREFLAGS_ALPHATESTMIPGENERATION        = 0x00400000,

	TEXTUREFLAGS_NODEPTHBUFFER                 = 0x00800000,

	TEXTUREFLAGS_NICEFILTERED                  = 0x01000000,

	TEXTUREFLAGS_CLAMPU                        = 0x02000000,

	// xbox extensions
	TEXTUREFLAGS_PRESWIZZLED                   = 0x04000000,
	TEXTUREFLAGS_CACHEABLE                     = 0x08000000,
	TEXTUREFLAGS_UNFILTERABLE_OK               = 0x10000000,

	TEXTUREFLAGS_LASTFLAG                      = 0x10000000,
};


//-----------------------------------------------------------------------------
// Cubemap face indices
//-----------------------------------------------------------------------------
enum CubeMapFaceIndex_t
{
	CUBEMAP_FACE_RIGHT = 0,
	CUBEMAP_FACE_LEFT,
	CUBEMAP_FACE_BACK,	// NOTE: This face is in the +y direction?!?!?
	CUBEMAP_FACE_FRONT,	// NOTE: This face is in the -y direction!?!?
	CUBEMAP_FACE_UP,
	CUBEMAP_FACE_DOWN,

	// This is the fallback for low-end
	CUBEMAP_FACE_SPHEREMAP,

	// NOTE: Cubemaps have *7* faces; the 7th is the fallback spheremap
	CUBEMAP_FACE_COUNT
};


//-----------------------------------------------------------------------------
// Enumeration used for spheremap generation
//-----------------------------------------------------------------------------
enum LookDir_t
{
	LOOK_DOWN_X = 0,
	LOOK_DOWN_NEGX,
	LOOK_DOWN_Y,
	LOOK_DOWN_NEGY,
	LOOK_DOWN_Z,
	LOOK_DOWN_NEGZ,
};


//-----------------------------------------------------------------------------
// Use this image format if you want to perform tool operations on the texture
//-----------------------------------------------------------------------------
#define IMAGE_FORMAT_DEFAULT	((ImageFormat)-2)


//-----------------------------------------------------------------------------
// Interface to get at various bits of a VTF texture
//-----------------------------------------------------------------------------
class IVTFTexture
{
public:
	// Initializes the texture and allocates space for the bits
	// In most cases, you shouldn't force the mip count.
	virtual bool Init( int nWidth, int nHeight, int nDepth, ImageFormat fmt, int nFlags, int iFrameCount, int nForceMipCount = -1 ) = 0;

	// Methods to set other texture fields
	virtual void SetBumpScale( float flScale ) = 0;
	virtual void SetReflectivity( const Vector &vecReflectivity ) = 0;

	// Methods to initialize the low-res image
	virtual void InitLowResImage( int nWidth, int nHeight, ImageFormat fmt ) = 0;

	// When unserializing, we can skip a certain number of mip levels,
	// and we also can just load everything but the image data
	// NOTE: If you load only the buffer header, you'll need to use the
	// VTFBufferHeaderSize() method below to only read that much from the file
	// NOTE: If you skip mip levels, the height + width of the texture will
	// change to reflect the size of the largest read in mip level
#ifndef _XBOX
	virtual bool Unserialize( CUtlBuffer &buf, bool bBufferHeaderOnly = false, int nSkipMipLevels = 0 ) = 0;
	virtual bool Serialize( CUtlBuffer &buf ) = 0;
#else
	virtual bool Unserialize( CUtlBuffer &buf, bool bBufferHeaderOnly, bool bStaticDataOnly ) = 0;
#endif

	// These are methods to help with optimization:
	// Once the header is read in, they indicate where to start reading
	// other data (measured from file start), and how many bytes to read....
	virtual void LowResFileInfo( int *pStartLocation, int *pSizeInBytes) const = 0;
	virtual void ImageFileInfo( int nFrame, int nFace, int nMip, int *pStartLocation, int *pSizeInBytes) const = 0;
#ifndef _XBOX
	virtual int FileSize( int nMipSkipCount = 0 ) const = 0;
#else
	virtual int FileSize( bool bStaticOnly ) const = 0;
	virtual int FileImageDataOffset( void ) const = 0;
	virtual int FileImageDataLength( int numFrames ) const = 0;
#endif

	// Attributes...
	virtual int Width() const = 0;
	virtual int Height() const = 0;
	virtual int Depth() const = 0;
	virtual int MipCount() const = 0;

	// returns the size of one row of a particular mip level
	virtual int RowSizeInBytes( int nMipLevel ) const = 0;

	// returns the size of one face of a particular mip level
	virtual int FaceSizeInBytes( int nMipLevel ) const = 0;

	virtual ImageFormat Format() const = 0;
	virtual int FaceCount() const = 0;
	virtual int FrameCount() const = 0;
	virtual int Flags() const = 0;

	virtual float BumpScale() const = 0;

	virtual int LowResWidth() const = 0;
	virtual int LowResHeight() const = 0;
	virtual ImageFormat LowResFormat() const = 0;

#ifdef _XBOX
	virtual int FallbackWidth() const = 0;
	virtual int FallbackHeight() const = 0;
	virtual int FallbackMipCount() const = 0;

	virtual int MappingWidth() const = 0;
	virtual int MappingHeight() const = 0;
	virtual int MipSkipCount() const = 0;
#endif

	// NOTE: reflectivity[0] = blue, [1] = greem, [2] = red
	virtual const Vector &Reflectivity() const = 0;

	virtual bool IsCubeMap() const = 0;
	virtual bool IsNormalMap() const = 0;
	virtual bool IsVolumeTexture() const = 0;
#ifdef _XBOX
	virtual bool IsPreSwizzled() const = 0;
#endif

	// Computes the dimensions of a particular mip level
	virtual void ComputeMipLevelDimensions( int iMipLevel, int *pMipWidth, int *pMipHeight, int *pMipDepth ) const = 0;

#ifdef _XBOX
	virtual void ComputeFallbackMipDimensions( int iMipLevel, int *pMipWidth, int *pMipHeight ) const = 0;
#endif
	// Computes the size (in bytes) of a single mipmap of a single face of a single frame 
	virtual int ComputeMipSize( int iMipLevel ) const = 0;

	// Computes the size of a subrect (specified at the top mip level) at a particular lower mip level
	virtual void ComputeMipLevelSubRect( Rect_t* pSrcRect, int nMipLevel, Rect_t *pSubRect ) const = 0;

	// Computes the size (in bytes) of a single face of a single frame
	// All mip levels starting at the specified mip level are included
	virtual int ComputeFaceSize( int iStartingMipLevel = 0 ) const = 0;

	// Computes the total size (in bytes) of all faces, all frames
	virtual int ComputeTotalSize() const = 0;

	// Returns the base address of the image data
	virtual unsigned char *ImageData() = 0;

	// Returns a pointer to the data associated with a particular frame, face, and mip level
	virtual unsigned char *ImageData( int iFrame, int iFace, int iMipLevel ) = 0;

	// Returns a pointer to the data associated with a particular frame, face, mip level, and offset
	virtual unsigned char *ImageData( int iFrame, int iFace, int iMipLevel, int x, int y, int z = 0 ) = 0;

#ifdef _XBOX
	// Returns a pointer to the palette data associated with a particular frame
	virtual unsigned char *PaletteData( int iFrame ) = 0;

	// Returns a pointer to the fallback data associated with a particular frame, face, and mip level
	virtual unsigned char *FallbackImageData( int iFrame, int iFace, int iMipLevel ) = 0;
#endif
	// Returns the base address of the low-res image data
	virtual unsigned char *LowResImageData() = 0;

	// Converts the textures image format. Use IMAGE_FORMAT_DEFAULT
	// if you want to be able to use various tool functions below
	virtual	void ConvertImageFormat( ImageFormat fmt, bool bNormalToDUDV ) = 0;

	// NOTE: The following methods only work on textures using the
	// IMAGE_FORMAT_DEFAULT!

	// Generate spheremap based on the current cube faces (only works for cubemaps)
	// The look dir indicates the direction of the center of the sphere
	// NOTE: Only call this *after* cube faces have been correctly
	// oriented (using FixCubemapFaceOrientation)
#ifndef _XBOX
	virtual void GenerateSpheremap( LookDir_t lookDir = LOOK_DOWN_Z ) = 0;

	// Generate spheremap based on the current cube faces (only works for cubemaps)
	// The look dir indicates the direction of the center of the sphere
	// NOTE: Only call this *after* cube faces have been correctly
	// oriented (using FixCubemapFaceOrientation)
	virtual void GenerateHemisphereMap( unsigned char *pSphereMapBitsRGBA, int targetWidth, 
		int targetHeight, LookDir_t lookDir, int iFrame ) = 0;

	// Fixes the cubemap faces orientation from our standard to the
	// standard the material system needs.
	virtual void FixCubemapFaceOrientation( ) = 0;

	// Generates mipmaps from the base mip levels
	virtual void GenerateMipmaps() = 0;

	// Put 1/miplevel (1..n) into alpha.
	virtual void PutOneOverMipLevelInAlpha() = 0;
	
	// Computes the reflectivity
	virtual void ComputeReflectivity( ) = 0;

	// Computes the alpha flags
	virtual void ComputeAlphaFlags() = 0;

	// Generate the low-res image bits
	virtual bool ConstructLowResImage() = 0;

	// Gets the texture all internally consistent assuming you've loaded
	// mip 0 of all faces of all frames
	virtual void PostProcess(bool bGenerateSpheremap, LookDir_t lookDir = LOOK_DOWN_Z, bool bAllowFixCubemapOrientation = true) = 0;

	// Blends adjacent pixels on cubemap borders, since the card doesn't do it. If the texture
	// is S3TC compressed, then it has to do it AFTER the texture has been compressed to prevent
	// artifacts along the edges.
	//
	// If bSkybox is true, it assumes the faces are oriented in the way the engine draws the skybox
	// (which happens to be different from the way cubemaps have their faces).
	virtual void MatchCubeMapBorders( int iStage, ImageFormat finalFormat, bool bSkybox ) = 0;

	// Sets threshhold values for alphatest mipmapping
	virtual void SetAlphaTestThreshholds( float flBase, float flHighFreq ) = 0;
#endif

#ifdef _XBOX
	virtual void ReleaseMemory() = 0;
#endif
};


//-----------------------------------------------------------------------------
// Class factory
//-----------------------------------------------------------------------------
IVTFTexture *CreateVTFTexture();
void DestroyVTFTexture( IVTFTexture *pTexture );


//-----------------------------------------------------------------------------
// Allows us to only load in the first little bit of the VTF file to get info
// Clients should read this much into a UtlBuffer and then pass it in to
// Unserialize
//-----------------------------------------------------------------------------
int VTFFileHeaderSize( int nMajorVersion = -1, int nMinorVersion = -1 );
#include "vector.h"

#endif // XTF_FILE_FORMAT_ONLY

//-----------------------------------------------------------------------------
// Disk format for VTF files
//
// NOTE: After the header is the low-res image data
// Then follows image data, which is sorted in the following manner
//
//	for each mip level (starting with 1x1, and getting larger)
//		for each animation frame
//			for each face
//				store the image data for the face
//
// NOTE: In memory, we store the data in the following manner:
//	for each animation frame
//		for each face
//			for each mip level (starting with the largest, and getting smaller)
//				store the image data for the face
//
// This is done because the various image manipulation function we have
// expect this format
//-----------------------------------------------------------------------------

#pragma pack(1)

// version number for the disk texture cache
#define VTF_MAJOR_VERSION 7
#define VTF_MINOR_VERSION 2

struct VTFFileBaseHeader_t
{
	char fileTypeString[4]; // "VTF" Valve texture file
	int version[2]; 		// version[0].version[1]
	int headerSize;
};

struct VTFFileHeaderV7_1_t : public VTFFileBaseHeader_t 
{
	unsigned short	width;
	unsigned short	height;
	unsigned int	flags;
	unsigned short	numFrames;
	unsigned short	startFrame;
	VectorAligned	reflectivity; // This is a linear value, right?  Average of all frames?
	float			bumpScale;
	ImageFormat		imageFormat;
	unsigned char	numMipLevels;
	ImageFormat		lowResImageFormat;
	unsigned char	lowResImageWidth;
	unsigned char	lowResImageHeight;	
};

struct VTFFileHeader_t : public VTFFileHeaderV7_1_t
{
	unsigned short depth;
};


#define XTF_MAJOR_VERSION	5
#define XTF_MINOR_VERSION	0

struct XTFFileHeader_t : public VTFFileBaseHeader_t 
{
	unsigned int	flags;
	unsigned short	width;					// actual width
	unsigned short	height;					// actual height
	unsigned short	depth;					// always 1
	unsigned short	numFrames;
	unsigned short	preloadDataSize;		// exact size of preload data
	unsigned short	imageDataOffset;		// aligned to sector size
	Vector			reflectivity;			// Resides on 16 byte boundary!
	float			bumpScale;
	ImageFormat		imageFormat;
	unsigned char	lowResImageWidth;
	unsigned char	lowResImageHeight;
	unsigned char	fallbackImageWidth;
	unsigned char	fallbackImageHeight;
	unsigned char	mipSkipCount;			// used to resconstruct mapping dimensions
	unsigned char	pad;					// for alignment
};

#pragma pack()

#endif // VTF_H
