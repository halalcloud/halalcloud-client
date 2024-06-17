namespace BaiduBce.Auth;

public interface IBceSessionCredentials : IBceCredentials
{
	string SessionToken { get; }
}
