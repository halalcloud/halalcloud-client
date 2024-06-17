using System;
using System.Globalization;
using System.Net;
using System.Reflection;
using System.Text.RegularExpressions;
using BaiduBce.Auth;
using Microsoft.Win32;

namespace BaiduBce;

public class BceClientConfiguration
{
	public const int DefaultTimeoutInMillis = 50000;

	public const int DefaultReadWriteTimeoutInMillis = 50000;

	public const int DefaultSocketBufferSizeInBytes = 8192;

	public static readonly ISigner DefaultSigner = new BceV1Signer();

	private static readonly string DefaultUserAgent = GenerateDefaultUserAgent();

	public IBceCredentials Credentials { get; set; }

	public string Region { get; set; }

	public string Endpoint { get; set; }

	public string Protocol { get; set; }

	public int? TimeoutInMillis { get; set; }

	public int? ReadWriteTimeoutInMillis { get; set; }

	public string ProxyHost { get; set; }

	public int? ProxyPort { get; set; }

	public ICredentials ProxyCredentials { get; set; }

	public bool? UseNagleAlgorithm { get; set; }

	public int? MaxIdleTimeInMillis { get; set; }

	public int? ConnectionLimit { get; set; }

	public int? SocketBufferSizeInBytes { get; set; }

	public ISigner Signer { get; set; }

	public SignOptions SignOptions { get; set; }

	public IRetryPolicy RetryPolicy { get; set; }

	public bool? CnameEnabled { get; set; }

	public bool? PathStyleEnabled { get; set; }

	public string UserAgent => DefaultUserAgent;

	public static int DefaultMaxIdleTimeInMillis
	{
		get
		{
			if (ServicePointManager.MaxServicePointIdleTime == 100000)
			{
				return 50000;
			}
			return ServicePointManager.MaxServicePointIdleTime;
		}
	}

	public static int DefaultConnectionLimit
	{
		get
		{
			if (ServicePointManager.DefaultConnectionLimit == 2)
			{
				return 50;
			}
			return ServicePointManager.DefaultConnectionLimit;
		}
	}

	public BceClientConfiguration()
	{
	}

	public BceClientConfiguration(BceClientConfiguration other)
	{
		Credentials = other.Credentials;
		Region = other.Region;
		Endpoint = other.Endpoint;
		Protocol = other.Protocol;
		TimeoutInMillis = other.TimeoutInMillis;
		ReadWriteTimeoutInMillis = other.ReadWriteTimeoutInMillis;
		ProxyHost = other.ProxyHost;
		ProxyPort = other.ProxyPort;
		ProxyCredentials = other.ProxyCredentials;
		UseNagleAlgorithm = other.UseNagleAlgorithm;
		MaxIdleTimeInMillis = other.MaxIdleTimeInMillis;
		ConnectionLimit = other.ConnectionLimit;
		SocketBufferSizeInBytes = other.SocketBufferSizeInBytes;
		Signer = other.Signer;
		SignOptions = other.SignOptions;
		RetryPolicy = other.RetryPolicy;
		CnameEnabled = other.CnameEnabled;
		PathStyleEnabled = other.PathStyleEnabled;
	}

	public BceClientConfiguration Merge(BceClientConfiguration other)
	{
		BceClientConfiguration bceClientConfiguration = new BceClientConfiguration(this);
		if (other == null)
		{
			return bceClientConfiguration;
		}
		if (other.Credentials != null)
		{
			bceClientConfiguration.Credentials = other.Credentials;
		}
		if (other.Region != null)
		{
			bceClientConfiguration.Region = other.Region;
		}
		if (other.Endpoint != null)
		{
			bceClientConfiguration.Endpoint = other.Endpoint;
		}
		if (other.Protocol != null)
		{
			bceClientConfiguration.Protocol = other.Protocol;
		}
		if (other.TimeoutInMillis.HasValue)
		{
			bceClientConfiguration.TimeoutInMillis = other.TimeoutInMillis;
		}
		if (other.ReadWriteTimeoutInMillis.HasValue)
		{
			bceClientConfiguration.ReadWriteTimeoutInMillis = other.ReadWriteTimeoutInMillis;
		}
		if (other.ProxyHost != null)
		{
			bceClientConfiguration.ProxyHost = other.ProxyHost;
		}
		if (other.ProxyPort.HasValue)
		{
			bceClientConfiguration.ProxyPort = other.ProxyPort;
		}
		if (other.ProxyCredentials != null)
		{
			bceClientConfiguration.ProxyCredentials = other.ProxyCredentials;
		}
		if (other.UseNagleAlgorithm.HasValue)
		{
			bceClientConfiguration.UseNagleAlgorithm = other.UseNagleAlgorithm;
		}
		if (other.MaxIdleTimeInMillis.HasValue)
		{
			bceClientConfiguration.MaxIdleTimeInMillis = other.MaxIdleTimeInMillis;
		}
		if (other.ConnectionLimit.HasValue)
		{
			bceClientConfiguration.ConnectionLimit = other.ConnectionLimit;
		}
		if (other.SocketBufferSizeInBytes.HasValue)
		{
			bceClientConfiguration.SocketBufferSizeInBytes = other.SocketBufferSizeInBytes;
		}
		if (other.Signer != null)
		{
			bceClientConfiguration.Signer = other.Signer;
		}
		if (other.SignOptions != null)
		{
			bceClientConfiguration.SignOptions = other.SignOptions;
		}
		if (other.RetryPolicy != null)
		{
			bceClientConfiguration.RetryPolicy = other.RetryPolicy;
		}
		if (other.CnameEnabled.HasValue)
		{
			bceClientConfiguration.CnameEnabled = other.CnameEnabled;
		}
		if (other.PathStyleEnabled.HasValue)
		{
			bceClientConfiguration.PathStyleEnabled = other.PathStyleEnabled;
		}
		return bceClientConfiguration;
	}

	internal static BceClientConfiguration CreateWithDefaultValues()
	{
		return new BceClientConfiguration
		{
			Region = "bj",
			Protocol = "http",
			TimeoutInMillis = 50000,
			ReadWriteTimeoutInMillis = 50000,
			UseNagleAlgorithm = true,
			MaxIdleTimeInMillis = DefaultMaxIdleTimeInMillis,
			ConnectionLimit = DefaultConnectionLimit,
			SocketBufferSizeInBytes = 8192,
			CnameEnabled = false,
			PathStyleEnabled = false,
			Signer = new BceV1Signer(),
			SignOptions = new SignOptions(),
			RetryPolicy = new DefaultRetryPolicy()
		};
	}

	private static string GenerateDefaultUserAgent()
	{
		string text = "Unknown";
		try
		{
			text = Environment.OSVersion.ToString();
		}
		catch
		{
		}
		string text2 = string.Format(CultureInfo.InvariantCulture, "{0}.{1}", Environment.Version.Major, Environment.Version.MajorRevision);
		string input = $"bce-sdk-dotnet/{Assembly.GetExecutingAssembly().GetName().Version.ToString()}/Framework:{GetFrameworkVersion()}/Runtime:{text2}/OS:{text}";
		return new Regex("\\s+").Replace(input, "_");
	}

	private static string GetFrameworkVersion()
	{
		using (RegistryKey registryKey = Registry.LocalMachine.OpenSubKey("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\"))
		{
			if (registryKey == null)
			{
				return "Unknown(NDP key not found)";
			}
			using (RegistryKey registryKey2 = registryKey.OpenSubKey("v4"))
			{
				if (Environment.Version.Major >= 4 && registryKey2 != null)
				{
					return GetFrameworkVersionAfter4(registryKey2);
				}
			}
			using (RegistryKey registryKey3 = registryKey.OpenSubKey("v3.5"))
			{
				if (registryKey3 != null)
				{
					return "3.5";
				}
			}
			using (RegistryKey registryKey4 = registryKey.OpenSubKey("v3.0"))
			{
				if (registryKey4 != null)
				{
					return "3.0";
				}
			}
			using RegistryKey registryKey5 = registryKey.OpenSubKey("v2.0.50727");
			if (registryKey5 != null)
			{
				return "2.0";
			}
		}
		return "Unknown";
	}

	private static string GetFrameworkVersionAfter4(RegistryKey v4Key)
	{
		using (RegistryKey registryKey = v4Key.OpenSubKey("Full"))
		{
			if (registryKey != null)
			{
				object value = registryKey.GetValue("Release");
				if (value != null)
				{
					int num = Convert.ToInt32(value);
					if (num > 393273)
					{
						return ">4.6RC";
					}
					if (num == 393273)
					{
						return "4.6RC";
					}
					if (num >= 379893)
					{
						return "4.5.2";
					}
					if (num >= 378675)
					{
						return "4.5.1";
					}
					if (num >= 378389)
					{
						return "4.5";
					}
				}
			}
		}
		return "4.0";
	}
}
