using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BaiduBce.Util;

public static class HttpUtils
{
	private static readonly string[] PercentEncodedStrings;

	static HttpUtils()
	{
		PercentEncodedStrings = (from v in Enumerable.Range(0, 256)
			select "%" + v.ToString("X2")).ToArray();
		for (char c = 'a'; c <= 'z'; c = (char)(c + 1))
		{
			PercentEncodedStrings[(uint)c] = c.ToString();
		}
		for (char c2 = 'A'; c2 <= 'Z'; c2 = (char)(c2 + 1))
		{
			PercentEncodedStrings[(uint)c2] = c2.ToString();
		}
		for (char c3 = '0'; c3 <= '9'; c3 = (char)(c3 + 1))
		{
			PercentEncodedStrings[(uint)c3] = c3.ToString();
		}
		PercentEncodedStrings[45] = "-";
		PercentEncodedStrings[46] = ".";
		PercentEncodedStrings[95] = "_";
		PercentEncodedStrings[126] = "~";
	}

	public static string NormalizePath(string path)
	{
		return Normalize(path).Replace("%2F", "/");
	}

	public static string Normalize(string value)
	{
		StringBuilder stringBuilder = new StringBuilder();
		byte[] bytes = Encoding.UTF8.GetBytes(value);
		foreach (byte b in bytes)
		{
			stringBuilder.Append(PercentEncodedStrings[b & 0xFF]);
		}
		return stringBuilder.ToString();
	}

	public static string GenerateHostHeader(Uri uri)
	{
		string text = uri.Host;
		if (IsUsingNonDefaultPort(uri))
		{
			text = text + ":" + uri.Port;
		}
		return text;
	}

	public static bool IsUsingNonDefaultPort(Uri uri)
	{
		string text = uri.Scheme.ToLower();
		int port = uri.Port;
		if (port <= 0)
		{
			return false;
		}
		if (text == "http")
		{
			return port != 80;
		}
		if (text == "https")
		{
			return port != 443;
		}
		return false;
	}

	public static string GetCanonicalQueryString(IDictionary<string, string> parameters, bool forSignature)
	{
		if (parameters.Count == 0)
		{
			return "";
		}
		List<string> list = new List<string>();
		foreach (KeyValuePair<string, string> parameter in parameters)
		{
			string key = parameter.Key;
			if (forSignature && "Authorization".Equals(key, StringComparison.OrdinalIgnoreCase))
			{
				continue;
			}
			if (key == null)
			{
				throw new ArgumentNullException("parameter key should NOT be null");
			}
			string value = parameter.Value;
			if (value == null)
			{
				if (forSignature)
				{
					list.Add(Normalize(key) + "=");
				}
				else
				{
					list.Add(Normalize(key));
				}
			}
			else
			{
				list.Add(Normalize(key) + "=" + Normalize(value));
			}
		}
		list.Sort();
		return string.Join("&", list.ToArray());
	}

	public static string AppendUri(string baseUri, params string[] pathComponents)
	{
		if (pathComponents.Length == 0)
		{
			return baseUri;
		}
		StringBuilder stringBuilder = new StringBuilder(baseUri.ToString().TrimEnd('/'));
		for (int i = 0; i < pathComponents.Length; i++)
		{
			string text = pathComponents[i];
			if (!string.IsNullOrEmpty(text))
			{
				if (i < pathComponents.Length - 1)
				{
					text = text.TrimEnd('/');
				}
				if (!text.StartsWith("/"))
				{
					stringBuilder.Append('/');
				}
				stringBuilder.Append(NormalizePath(text));
			}
		}
		return stringBuilder.ToString();
	}
}
