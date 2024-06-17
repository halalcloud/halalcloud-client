using System;
using System.IO;
using System.Security.Cryptography;

namespace BaiduBce.Util;

public static class HashUtils
{
	public static string ComputeSHA256Hash(FileInfo fileInfo)
	{
		using Stream sourceStream = fileInfo.OpenRead();
		byte[] buffer = IOUtils.StreamToBytes(sourceStream);
		return Convert.ToBase64String(new SHA256Managed().ComputeHash(buffer));
	}

	public static string ComputeMD5Hash(FileInfo fileInfo)
	{
		using MD5 mD = MD5.Create();
		using FileStream inputStream = fileInfo.OpenRead();
		return BitConverter.ToString(mD.ComputeHash(inputStream)).Replace("-", "").ToLower();
	}

	public static string ComputeMD5HashWithBase64(FileInfo fileInfo)
	{
		using MD5 mD = MD5.Create();
		using FileStream inputStream = fileInfo.OpenRead();
		return Convert.ToBase64String(mD.ComputeHash(inputStream));
	}

	public static string ComputeMD5Hash(Stream stream, long contentLength)
	{
		using MD5 mD = MD5.Create();
		long position = stream.Position;
		byte[] array = new byte[4096];
		long num = 0L;
		int num2 = 0;
		while (num < contentLength)
		{
			num2 = (int)((contentLength - num > 4096) ? 4096 : (contentLength - num));
			num2 = stream.Read(array, 0, num2);
			if (num2 <= 0)
			{
				break;
			}
			num += num2;
			mD.TransformBlock(array, 0, num2, array, 0);
		}
		mD.TransformFinalBlock(new byte[0], 0, 0);
		stream.Position = position;
		return BitConverter.ToString(mD.Hash).Replace("-", "").ToLower();
	}
}
