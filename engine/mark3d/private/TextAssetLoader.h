#ifndef __TEXTASSET_LOADER_H__
#define __TEXTASSET_LOADER_H__

class TextAsset;
interface IFileSystem;

extern BOOL LoadTextAssetFromFileSystem(
	IFileSystem* pFileSystem,
	const char* szRelativePath, 
	TextAsset* pTextAsset
);

extern void AsyncLoadTextAssetFromFileSystem(void* pArg);

#endif // __TEXTASSET_LOADER_H__