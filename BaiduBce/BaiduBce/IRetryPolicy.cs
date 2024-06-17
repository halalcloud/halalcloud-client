using System;

namespace BaiduBce;

public interface IRetryPolicy
{
	bool CanRetry { get; set; }

	T Execute<T>(Func<int, T> func);
}
