using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.IO;

namespace BaiduBce.Util;

public static class MimeTypes
{
	public const string MimeTypeOctetStream = "application/octet-stream";

    private static readonly ILogger Logger =
        LogUtils.Factory.CreateLogger(typeof(MimeTypes));

    private static IDictionary<string, string> extensionToMimetypeMap;

	static MimeTypes()
	{
		extensionToMimetypeMap = new Dictionary<string, string>();
		Stream manifestResourceStream = typeof(MimeTypes).Assembly.GetManifestResourceStream("BaiduBce.mime.types");
		if (manifestResourceStream != null)
		{
			using (manifestResourceStream)
			{
				LoadAndReplaceMimetypes(manifestResourceStream);
				return;
			}
		}
        Logger.LogWarning("Unable to find 'mime.types'");
	}

	private static void LoadAndReplaceMimetypes(Stream stream)
	{
		StreamReader streamReader = new StreamReader(stream);
		string text = null;
		while ((text = streamReader.ReadLine()) != null)
		{
			text = text.Trim();
			if (text.StartsWith("#") || text.Length == 0)
			{
				continue;
			}
			string[] array = text.Split(new char[2] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
			if (array.Length > 1)
			{
				string value = array[0];
				for (int i = 1; i < array.Length; i++)
				{
					string text2 = array[i];
					extensionToMimetypeMap[text2.ToLower()] = value;
				}
			}
		}
	}

	public static string GetMimetype(string extension)
	{
		if (string.IsNullOrEmpty(extension))
		{
			return "application/octet-stream";
		}
		if (extensionToMimetypeMap.TryGetValue(extension, out var value))
		{
			return value;
		}
		return "application/octet-stream";
	}

	public static string GetMimetype(FileInfo fileInfo)
	{
		string text = fileInfo.Extension;
		if (!string.IsNullOrEmpty(text))
		{
			text = text.TrimStart('.');
		}
		return GetMimetype(text);
	}
}
