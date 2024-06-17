using System;
using System.Globalization;

namespace BaiduBce.Util;

public static class DateUtils
{
	private static string rfc822DateFormat = "ddd, dd MMM yyyy HH:mm:ss 'GMT'";

	public static string FormatAlternateIso8601Date(DateTime date)
	{
		return date.ToUniversalTime().ToString("yyyy'-'MM'-'dd'T'HH':'mm':'ssK");
	}

	public static DateTime ParseRfc822Date(string dateString)
	{
		DateTime.TryParseExact(dateString, rfc822DateFormat, DateTimeFormatInfo.InvariantInfo, DateTimeStyles.AssumeUniversal, out var result);
		return result;
	}
}
