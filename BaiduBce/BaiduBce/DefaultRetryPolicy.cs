using BaiduBce.Util;
using Microsoft.Extensions.Logging;
using System;
using System.IO;
using System.Net;

namespace BaiduBce;

public class DefaultRetryPolicy : IRetryPolicy
{
    private static readonly ILogger Logger =
        LogUtils.Factory.CreateLogger<DefaultRetryPolicy>();

	public const int DefaultMaxErrorRetry = 3;

	public const int DefaultMaxDelayInMillis = 20000;

	public const int DefaultScaleFactorInMillis = 300;

	public int MaxErrorRetry { get; set; }

	public int MaxDelayInMillis { get; set; }

	public int ScaleFactorInMillis { get; set; }

	public bool CanRetry { get; set; }

	public DefaultRetryPolicy()
	{
		MaxErrorRetry = 3;
		MaxDelayInMillis = 20000;
		ScaleFactorInMillis = 300;
		CanRetry = true;
	}

	public T Execute<T>(Func<int, T> func)
	{
		int num = 1;
		while (true)
		{
			try
			{
				return func(num);
			}
			catch (Exception ex)
			{
                if (Logger.IsEnabled(LogLevel.Debug))
                {
                    Logger.LogDebug(ex, "Unable to execute request");
                }
                if (!(ex is BceBaseException))
				{
					ex = new BceClientException("Unable to execute request", ex);
				}
				if ((long)GetDelayBeforeNextRetryInMillis(ex, num) < 0L)
				{
					throw ex;
				}
			}
			num++;
		}
	}

	protected virtual int GetDelayBeforeNextRetryInMillis(Exception exception, int attempt)
	{
		int num = attempt - 1;
		int maxErrorRetry = MaxErrorRetry;
		if (num >= maxErrorRetry)
		{
			return -1;
		}
		if (!ShouldRetry(exception, num))
		{
			return -1;
		}
		return Math.Min(MaxDelayInMillis, (1 << num + 1) * ScaleFactorInMillis);
	}

	protected virtual bool ShouldRetry(Exception exception, int retriesAttempted)
	{
		if (!CanRetry)
		{
			return false;
		}
		if (exception.InnerException is WebException)
		{
            Logger.LogDebug("Retry for WebException.");
			return true;
		}
		if (exception.InnerException is IOException)
		{
            Logger.LogDebug("Retry for IOException.");
			return true;
		}
		if (exception is BceServiceException)
		{
			BceServiceException ex = exception as BceServiceException;
			if (ex.StatusCode == 500)
			{
                Logger.LogDebug("Retry for internal server error.");
				return true;
			}
			if (ex.StatusCode == 503)
			{
                Logger.LogDebug("Retry for service unavailable.");
				return true;
			}
			if (ex.StatusCode == 400 && ex.ErrorCode == "Http400")
			{
                Logger.LogDebug("Retry for bad request.");
				return true;
			}
			if (ex.ErrorCode == "RequestExpired")
			{
                Logger.LogDebug("Retry for request expired.");
				return true;
			}
		}
		return false;
	}
}
