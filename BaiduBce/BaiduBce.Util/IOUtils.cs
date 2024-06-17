using System;
using System.IO;

namespace BaiduBce.Util;

public static class IOUtils
{
	public const int DefaultBufferSize = 8192;

	public static Stream GetMemoryStream(Stream sourceStream)
	{
		return GetMemoryStream(sourceStream, 8192);
	}

	public static Stream GetMemoryStream(Stream sourceStream, int bufferSize)
	{
		Stream stream = new MemoryStream();
		byte[] array = new byte[bufferSize];
		int num = 0;
		while ((num = sourceStream.Read(array, 0, array.Length)) > 0)
		{
			stream.Write(array, 0, num);
		}
		return stream;
	}

	public static byte[] StreamToBytes(Stream sourceStream)
	{
		return StreamToBytes(sourceStream, sourceStream.Length, 8192);
	}

	public static byte[] StreamToBytes(Stream sourceStream, long streamLength, int bufferSize)
	{
		byte[] array = new byte[streamLength];
		byte[] array2 = new byte[bufferSize];
		int num = 0;
		int num2 = 0;
		while ((num2 = sourceStream.Read(array2, 0, bufferSize)) > 0)
		{
			Array.Copy(array2, 0, array, num, num2);
			num += num2;
		}
		return array;
	}

	public static void StreamToFile(Stream sourceStream, FileInfo destinationFileInfo, int bufferSize)
	{
		DirectoryInfo directory = destinationFileInfo.Directory;
		if (directory != null && !directory.Exists)
		{
			directory.Create();
		}
		using FileStream fileStream = destinationFileInfo.Create();
		byte[] array = new byte[bufferSize];
		int count;
		while ((count = sourceStream.Read(array, 0, array.Length)) > 0)
		{
			fileStream.Write(array, 0, count);
		}
	}
}
