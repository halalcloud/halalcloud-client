using BaiduBce.Internal;

namespace BaiduBce.Auth;

public interface ISigner
{
	string Sign(InternalRequest request);
}
