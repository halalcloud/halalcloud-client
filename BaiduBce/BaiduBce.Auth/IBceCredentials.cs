namespace BaiduBce.Auth;

public interface IBceCredentials
{
	string AccessKeyId { get; }

	string SecretKey { get; }
}
