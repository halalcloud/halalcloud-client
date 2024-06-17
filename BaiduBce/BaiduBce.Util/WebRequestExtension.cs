using System;
using System.IO;
using System.Net;

namespace BaiduBce.Util;

public static class WebRequestExtension
{
	public static Stream GetRequestStreamWithTimeout(WebRequest request, int? millisecondsTimeout = null)
	{
		return AsyncToSyncWithTimeout(request.BeginGetRequestStream, request.EndGetRequestStream, millisecondsTimeout ?? request.Timeout);
	}

	public static WebResponse GetResponseWithTimeout(HttpWebRequest request, int? millisecondsTimeout = null)
	{
		return AsyncToSyncWithTimeout(request.BeginGetResponse, request.EndGetResponse, millisecondsTimeout ?? request.Timeout);
	}

	private static T AsyncToSyncWithTimeout<T>(Func<AsyncCallback, object, IAsyncResult> begin, Func<IAsyncResult, T> end, int millisecondsTimeout)
	{
		IAsyncResult asyncResult = begin(null, null);
		if (!asyncResult.AsyncWaitHandle.WaitOne(millisecondsTimeout))
		{
			TimeoutException ex = new TimeoutException();
			throw new WebException(ex.Message, ex, WebExceptionStatus.Timeout, null);
		}
		return end(asyncResult);
	}
}
