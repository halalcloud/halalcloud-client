using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using System.Web;
using BaiduBce.Auth;
using BaiduBce.Internal;
using BaiduBce.Model;
using BaiduBce.Services.Bos.Model;
using BaiduBce.Util;
using Microsoft.Extensions.Logging;

namespace BaiduBce.Services.Bos;

public class BosClient : BceClientBase
{
	private const string UrlPrefix = "/v1";

	private const string serviceEndpointFormat = "{0}://{1}.bcebos.com";

    private static readonly ILogger Logger =
       LogUtils.Factory.CreateLogger<BosClient>();

    public BosClient()
		: this(new BceClientConfiguration())
	{
	}

	public BosClient(BceClientConfiguration config)
		: base(config, "{0}://{1}.bcebos.com")
	{
	}

	public User GetBosAccountOwner()
	{
		return ListBuckets().Owner;
	}

	public CreateBucketResponse CreateBucket(string bucketName)
	{
		return CreateBucket(new CreateBucketRequest
		{
			BucketName = bucketName
		});
	}

	public CreateBucketResponse CreateBucket(CreateBucketRequest request)
	{
		CheckNotNull(request, "request should NOT be null.");
		InternalRequest internalRequest = CreateInternalRequest("PUT", request);
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<CreateBucketResponse>(httpWebResponse);
			}
		});
	}

	public ListBucketsResponse ListBuckets()
	{
		return ListBuckets(new ListBucketsRequest());
	}

	public ListBucketsResponse ListBuckets(ListBucketsRequest request)
	{
		CheckNotNull(request, "request should NOT be null.");
		InternalRequest internalRequest = CreateInternalRequest("GET", request);
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<ListBucketsResponse>(httpWebResponse);
			}
		});
	}

	public void DeleteBucket(string bucketName)
	{
		DeleteBucket(new DeleteBucketRequest
		{
			BucketName = bucketName
		});
	}

	public void DeleteBucket(DeleteBucketRequest request)
	{
		CheckNotNull(request, "request should NOT be null.");
		InternalRequest internalRequest = CreateInternalRequest("DELETE", request);
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<DeleteBucketResponse>(httpWebResponse);
			}
		});
	}

	public bool DoesBucketExist(string bucketName)
	{
		return DoesBucketExist(new DoesBucketExistRequest
		{
			BucketName = bucketName
		});
	}

	public bool DoesBucketExist(DoesBucketExistRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		try
		{
			InternalRequest internalRequest = CreateInternalRequest("HEAD", request);
			return internalRequest.Config.RetryPolicy.Execute(delegate
			{
				using (httpClient.Execute(internalRequest))
				{
					return true;
				}
			});
		}
		catch (BceServiceException ex)
		{
			if (ex.StatusCode == 403)
			{
				return true;
			}
			if (ex.StatusCode == 404)
			{
				return false;
			}
			throw ex;
		}
	}

	public GetBucketAclResponse GetBucketAcl(string bucketName)
	{
		return GetBucketAcl(new BucketRequestBase
		{
			BucketName = bucketName
		});
	}

	public GetBucketAclResponse GetBucketAcl(BucketRequestBase request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("GET", request);
		internalRequest.Parameters["acl"] = null;
		GetBucketAclResponse getBucketAclResponse = internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<GetBucketAclResponse>(httpWebResponse);
			}
		});
		if (getBucketAclResponse.Version > 1)
		{
			throw new BceClientException("Unsupported acl version.");
		}
		return getBucketAclResponse;
	}

	public void SetBucketAcl(string bucketName, string acl)
	{
		SetBucketAcl(new SetBucketAclRequest
		{
			BucketName = bucketName,
			CannedAcl = acl
		});
	}

	public void SetBucketAcl(SetBucketAclRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("PUT", request);
		internalRequest.Parameters["acl"] = null;
		if (!string.IsNullOrEmpty(request.CannedAcl))
		{
			internalRequest.Headers["x-bce-acl"] = request.CannedAcl;
			internalRequest.Headers["Content-Length"] = "0";
		}
		else if (request.AccessControlList != null)
		{
			string json = JsonUtils.SerializeObject(new AccessControlListRequest
			{
				AccessControlList = request.AccessControlList
			});
			FillRequestBodyForJson(internalRequest, json);
		}
		else
		{
			CheckNotNull(null, "request.acl should not be null.");
		}
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BosResponseBase>(httpWebResponse);
			}
		});
	}

	public GetBucketLocationResponse GetBucketLocation(string bucketName)
	{
		return GetBucketLocation(new BucketRequestBase
		{
			BucketName = bucketName
		});
	}

	public GetBucketLocationResponse GetBucketLocation(BucketRequestBase request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("GET", request);
		internalRequest.Parameters["location"] = null;
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<GetBucketLocationResponse>(httpWebResponse);
			}
		});
	}

	public Uri GeneratePresignedUrl(string bucketName, string key, int expirationInSeconds)
	{
		return GeneratePresignedUrl(bucketName, key, expirationInSeconds, "GET");
	}

	public Uri GeneratePresignedUrl(string bucketName, string key, int expirationInSeconds, string method)
	{
		GeneratePresignedUrlRequest request = new GeneratePresignedUrlRequest
		{
			BucketName = bucketName,
			Key = key,
			Method = method,
			ExpirationInSeconds = expirationInSeconds
		};
		return GeneratePresignedUrl(request);
	}

	public Uri GeneratePresignedUrl(GeneratePresignedUrlRequest request)
	{
		CheckNotNull(request, "The request parameter must be specified when generating a pre-signed URL");
		string method = request.Method;
		if (!"GET".Equals(method, StringComparison.CurrentCultureIgnoreCase))
		{
			throw new ArgumentException("only support http method get");
		}
		InternalRequest internalRequest = new InternalRequest();
		BceClientConfiguration bceClientConfiguration = config.Merge(request.Config);
		bceClientConfiguration.SignOptions.ExpirationInSeconds = request.ExpirationInSeconds;
		internalRequest.Config = bceClientConfiguration;
		internalRequest.Uri = new Uri(HttpUtils.AppendUri(ComputeEndpoint(bceClientConfiguration), "/v1", request.BucketName, request.Key));
		internalRequest.HttpMethod = method;
		internalRequest.Headers["Host"] = HttpUtils.GenerateHostHeader(internalRequest.Uri);
		new SignOptions();
		foreach (KeyValuePair<string, string> requestHeader in request.RequestHeaders)
		{
			internalRequest.Headers[requestHeader.Key] = requestHeader.Value ?? "";
		}
		foreach (KeyValuePair<string, string> requestParameter in request.RequestParameters)
		{
			internalRequest.Parameters[requestParameter.Key] = requestParameter.Value ?? "";
		}
		if (request.ContentType != null)
		{
			internalRequest.Headers["Content-Type"] = request.ContentType;
		}
		if (request.ContentMd5 != null)
		{
			internalRequest.Headers["Content-MD5"] = request.ContentMd5;
		}
		internalRequest.Headers["Authorization"] = bceClientConfiguration.Signer.Sign(internalRequest);
		return ConvertRequestToUri(internalRequest);
	}

	public ListObjectsResponse ListObjects(string bucketName)
	{
		return ListObjects(new ListObjectsRequest
		{
			BucketName = bucketName
		});
	}

	public ListObjectsResponse ListObjects(string bucketName, string prefix)
	{
		return ListObjects(new ListObjectsRequest
		{
			BucketName = bucketName,
			Prefix = prefix
		});
	}

	public ListObjectsResponse ListObjects(ListObjectsRequest request)
	{
		CheckNotNull(request, "request should NOT be null.");
		InternalRequest internalRequest = CreateInternalRequest("GET", request);
		if (request.Prefix != null)
		{
			internalRequest.Parameters["prefix"] = request.Prefix;
		}
		if (request.Marker != null)
		{
			internalRequest.Parameters["marker"] = request.Marker;
		}
		if (request.Delimiter != null)
		{
			internalRequest.Parameters["delimiter"] = request.Delimiter;
		}
		if (request.MaxKeys.HasValue && request.MaxKeys >= 0)
		{
			internalRequest.Parameters["maxKeys"] = request.MaxKeys.ToString();
		}
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				ListObjectsResponse listObjectsResponse = ToObject<ListObjectsResponse>(httpWebResponse);
				if (listObjectsResponse != null)
				{
					listObjectsResponse.BucketName = request.BucketName;
					List<BosObjectSummary> contents = listObjectsResponse.Contents;
					if (contents != null && contents.Count > 0)
					{
						foreach (BosObjectSummary item in contents)
						{
							item.BucketName = request.BucketName;
							if (item.StorageClass == null)
							{
								item.StorageClass = "STANDARD";
							}
						}
					}
				}
				return listObjectsResponse;
			}
		});
	}

	public ListObjectsResponse ListNextBatchOfObjects(ListObjectsResponse previousResponse)
	{
		CheckNotNull(previousResponse, "response should NOT be null.");
		if (!previousResponse.IsTruncated)
		{
			return new ListObjectsResponse
			{
				BucketName = previousResponse.BucketName,
				Delimiter = previousResponse.Delimiter,
				Marker = previousResponse.NextMarker,
				MaxKeys = previousResponse.MaxKeys,
				Prefix = previousResponse.Prefix,
				IsTruncated = false
			};
		}
		return ListObjects(new ListObjectsRequest
		{
			BucketName = previousResponse.BucketName,
			Prefix = previousResponse.Prefix,
			Marker = previousResponse.NextMarker,
			Delimiter = previousResponse.Delimiter,
			MaxKeys = previousResponse.MaxKeys
		});
	}

	public PutObjectResponse PutObject(string bucketName, string key, FileInfo fileInfo)
	{
		return PutObject(new PutObjectRequest
		{
			BucketName = bucketName,
			Key = key,
			FileInfo = fileInfo
		});
	}

	public PutObjectResponse PutObject(string bucketName, string key, FileInfo fileInfo, ObjectMetadata metadata)
	{
		return PutObject(new PutObjectRequest
		{
			BucketName = bucketName,
			Key = key,
			FileInfo = fileInfo,
			ObjectMetadata = metadata
		});
	}

	public PutObjectResponse PutObject(string bucketName, string key, string value)
	{
		return PutObject(bucketName, key, value, new ObjectMetadata());
	}

	public PutObjectResponse PutObject(string bucketName, string key, string value, ObjectMetadata metadata)
	{
		return PutObject(bucketName, key, Encoding.UTF8.GetBytes(value), metadata);
	}

	public PutObjectResponse PutObject(string bucketName, string key, byte[] value)
	{
		return PutObject(bucketName, key, value, new ObjectMetadata());
	}

	public PutObjectResponse PutObject(string bucketName, string key, byte[] value, ObjectMetadata metadata)
	{
		if (metadata.ContentLength == 0L)
		{
			metadata.ContentLength = value.Length;
		}
		return PutObject(new PutObjectRequest
		{
			BucketName = bucketName,
			Key = key,
			Stream = new MemoryStream(value),
			ObjectMetadata = metadata
		});
	}

	public PutObjectResponse PutObject(string bucketName, string key, Stream input)
	{
		return PutObject(bucketName, key, input, new ObjectMetadata());
	}

	public PutObjectResponse PutObject(string bucketName, string key, Stream input, ObjectMetadata metadata)
	{
		return PutObject(new PutObjectRequest
		{
			BucketName = bucketName,
			Key = key,
			Stream = input,
			ObjectMetadata = metadata
		});
	}

	public PutObjectResponse PutObject(PutObjectRequest request)
	{
		CheckNotNull(request, "request should NOT be null.");
		if (string.IsNullOrEmpty(request.Key))
		{
			throw new ArgumentNullException("object key should not be null or empty");
		}
		ObjectMetadata objectMetadata = request.ObjectMetadata;
		Stream stream = request.Stream;
		InternalRequest internalRequest = CreateInternalRequest("PUT", request);
		if (request.FileInfo != null)
		{
			FileInfo fileInfo = request.FileInfo;
			if (fileInfo.Length > 5368709120L)
			{
				throw new BceServiceException
				{
					ErrorMessage = "Your proposed upload exceeds the maximum allowed object size.",
					StatusCode = 400,
					ErrorCode = "EntityTooLarge"
				};
			}
			objectMetadata.ContentLength = fileInfo.Length;
			if (objectMetadata.ContentType == null)
			{
				objectMetadata.ContentType = MimeTypes.GetMimetype(fileInfo);
			}
			internalRequest.Content = fileInfo.OpenRead();
			objectMetadata.ContentMd5 = HashUtils.ComputeMD5HashWithBase64(fileInfo);
		}
		else
		{
			CheckNotNull(stream, "Either file or inputStream should be set for PutObjectRequest.");
			if (objectMetadata.ContentLength <= 0)
			{
                Logger.LogWarning("No content length specified for stream data.");
				objectMetadata.ContentLength = stream.Length;
			}
			else if (objectMetadata.ContentLength > stream.Length)
			{
				throw new ArgumentNullException("ContentLength should not be greater than stream length");
			}
			internalRequest.Content = stream;
		}
		internalRequest.Headers["Content-Length"] = objectMetadata.ContentLength.ToString();
		if (!string.IsNullOrEmpty(objectMetadata.StorageClass))
		{
			internalRequest.Headers["x-bce-storage-class"] = objectMetadata.StorageClass;
		}
		if (request.TrafficLimit != 0L)
		{
			internalRequest.Headers["x-bce-traffic-limit"] = request.TrafficLimit.ToString();
		}
		using (internalRequest.Content)
		{
			PopulateRequestMetadata(internalRequest, objectMetadata);
			internalRequest.Config.RetryPolicy.CanRetry = internalRequest.Content.CanSeek;
			return internalRequest.Config.RetryPolicy.Execute(delegate
			{
				HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
				using (httpWebResponse)
				{
					PutObjectResponse putObjectResponse = ToObject<PutObjectResponse>(httpWebResponse);
					putObjectResponse.ETAG = httpWebResponse.Headers["ETag"].Replace("\"", "");
					return putObjectResponse;
				}
			});
		}
	}

	public BosObject GetObject(string bucketName, string key)
	{
		return GetObject(new GetObjectRequest
		{
			BucketName = bucketName,
			Key = key
		});
	}

	public ObjectMetadata GetObject(string bucketName, string key, FileInfo destinationFile)
	{
		return GetObject(new GetObjectRequest
		{
			BucketName = bucketName,
			Key = key
		}, destinationFile);
	}

	public byte[] GetObjectContent(string bucketName, string key)
	{
		return GetObjectContent(new GetObjectRequest
		{
			BucketName = bucketName,
			Key = key
		});
	}

	public byte[] GetObjectContent(GetObjectRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequestForGetObject(request);
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return IOUtils.StreamToBytes(httpWebResponse.GetResponseStream(), httpWebResponse.ContentLength, config.SocketBufferSizeInBytes.Value);
			}
		});
	}

	public BosObject GetObject(GetObjectRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequestForGetObject(request);
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			return new BosObject
			{
				BucketName = request.BucketName,
				Key = request.Key,
				ObjectContent = httpWebResponse.GetResponseStream(),
				ObjectMetadata = GetObjectMetadata(httpWebResponse)
			};
		});
	}

	public ObjectMetadata GetObject(GetObjectRequest request, FileInfo destinationFileInfo)
	{
		CheckNotNull(request, "request should not be null.");
		CheckNotNull(destinationFileInfo, "destinationFile should not be null.");
		InternalRequest internalRequest = CreateInternalRequestForGetObject(request);
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				IOUtils.StreamToFile(httpWebResponse.GetResponseStream(), destinationFileInfo, config.SocketBufferSizeInBytes.Value);
				return GetObjectMetadata(httpWebResponse);
			}
		});
	}

	public ObjectMetadata GetObjectMetadata(string bucketName, string key)
	{
		return GetObjectMetadata(new ObjectRequestBase
		{
			BucketName = bucketName,
			Key = key
		});
	}

	public ObjectMetadata GetObjectMetadata(ObjectRequestBase request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("HEAD", request);
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return GetObjectMetadata(httpWebResponse);
			}
		});
	}

	public CopyObjectResponse CopyObject(string sourceBucketName, string sourceKey, string destinationBucketName, string destinationKey)
	{
		return CopyObject(new CopyObjectRequest
		{
			BucketName = destinationBucketName,
			SourceBucketName = sourceBucketName,
			Key = destinationKey,
			SourceKey = sourceKey
		});
	}

	public CopyObjectResponse CopyObject(CopyObjectRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		CheckNotNull(request.SourceBucketName, "source bucket should not be null or empty");
		CheckNotNull(request.SourceKey, "source object key should not be null or empty");
		InternalRequest internalRequest = CreateInternalRequest("PUT", request);
		string path = "/" + request.SourceBucketName + "/" + request.SourceKey;
		path = HttpUtils.NormalizePath(path);
		internalRequest.Headers["x-bce-copy-source"] = path;
		if (!string.IsNullOrEmpty(request.ETag))
		{
			internalRequest.Headers["x-bce-copy-source-if-match"] = "\"" + request.ETag + "\"";
		}
		ObjectMetadata newObjectMetadata = request.NewObjectMetadata;
		if (newObjectMetadata != null)
		{
			internalRequest.Headers["x-bce-metadata-directive"] = "replace";
			PopulateRequestMetadata(internalRequest, newObjectMetadata);
		}
		else
		{
			internalRequest.Headers["x-bce-metadata-directive"] = "copy";
		}
		internalRequest.Headers["Content-Length"] = "0";
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				string s = new StreamReader(httpWebResponse.GetResponseStream(), Encoding.UTF8).ReadToEnd();
				BceServiceException.BceErrorResponse bceErrorResponse = JsonUtils.ToObject<BceServiceException.BceErrorResponse>(new StreamReader(new MemoryStream(Encoding.UTF8.GetBytes(s))));
				if (bceErrorResponse != null && !string.IsNullOrEmpty(bceErrorResponse.Code))
				{
					throw new BceServiceException
					{
						ErrorCode = bceErrorResponse.Code,
						RequestId = bceErrorResponse.RequestId,
						StatusCode = 500
					};
				}
				CopyObjectResponse copyObjectResponse = JsonUtils.ToObject<CopyObjectResponse>(new StreamReader(new MemoryStream(Encoding.UTF8.GetBytes(s))));
				if (copyObjectResponse == null)
				{
					copyObjectResponse = new CopyObjectResponse();
				}
				copyObjectResponse.BceRequestId = httpWebResponse.Headers["x-bce-request-id"];
				copyObjectResponse.BosDebugId = httpWebResponse.Headers["x-bce-debug-id"];
				return copyObjectResponse;
			}
		});
	}

	public void DeleteObject(string bucketName, string key)
	{
		DeleteObject(new ObjectRequestBase
		{
			BucketName = bucketName,
			Key = key
		});
	}

	public void DeleteObject(ObjectRequestBase request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("DELETE", request);
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BosResponseBase>(httpWebResponse);
			}
		});
	}

	public InitiateMultipartUploadResponse InitiateMultipartUpload(string bucketName, string key)
	{
		return InitiateMultipartUpload(new InitiateMultipartUploadRequest
		{
			BucketName = bucketName,
			Key = key
		});
	}

	public InitiateMultipartUploadResponse InitiateMultipartUpload(InitiateMultipartUploadRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("POST", request);
		internalRequest.Parameters["uploads"] = null;
		internalRequest.Headers["Content-Length"] = "0";
		if (request.ObjectMetadata != null)
		{
			PopulateRequestMetadata(internalRequest, request.ObjectMetadata);
		}
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<InitiateMultipartUploadResponse>(httpWebResponse);
			}
		});
	}

	public UploadPartResponse UploadPart(UploadPartRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		if (request.PartSize > 5368709120L)
		{
			throw new BceClientException("PartNumber " + request.PartNumber + " : Part Size should not be more than 5GB.");
		}
		InternalRequest internalRequest = CreateInternalRequest("PUT", request);
		internalRequest.Parameters["uploadId"] = request.UploadId;
		internalRequest.Parameters["partNumber"] = request.PartNumber.ToString();
		internalRequest.Headers["Content-Length"] = request.PartSize.ToString();
		Stream inputStream = request.InputStream;
		if (inputStream.CanSeek && string.IsNullOrEmpty(request.Md5Digest))
		{
			request.Md5Digest = HashUtils.ComputeMD5Hash(inputStream, request.PartSize);
		}
		using (inputStream)
		{
			internalRequest.Content = inputStream;
			internalRequest.Config.RetryPolicy.CanRetry = internalRequest.Content.CanSeek;
			string text = internalRequest.Config.RetryPolicy.Execute(delegate
			{
				HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
				using (httpWebResponse)
				{
					return httpWebResponse.Headers["ETag"].Replace("\"", "");
				}
			});
			if (!text.Equals(request.Md5Digest, StringComparison.CurrentCultureIgnoreCase))
			{
				throw new BceClientException("Unable to verify integrity of data upload.  Client calculated content hash didn't match hash calculated by Baidu BOS.  You may need to delete the data stored in Baiddu BOS.");
			}
			return new UploadPartResponse
			{
				ETag = text,
				PartNumber = request.PartNumber
			};
		}
	}

	public ListPartsResponse ListParts(string bucketName, string key, string uploadId)
	{
		return ListParts(new ListPartsRequest
		{
			BucketName = bucketName,
			Key = key,
			UploadId = uploadId
		});
	}

	public ListPartsResponse ListParts(ListPartsRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("GET", request);
		internalRequest.Parameters["uploadId"] = request.UploadId;
		if (request.MaxParts.HasValue)
		{
			internalRequest.Parameters["maxParts"] = request.MaxParts.ToString();
		}
		internalRequest.Parameters["partNumberMarker"] = request.PartNumberMarker.ToString();
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				ListPartsResponse listPartsResponse = ToObject<ListPartsResponse>(httpWebResponse);
				listPartsResponse.BucketName = request.BucketName;
				return listPartsResponse;
			}
		});
	}

	public CompleteMultipartUploadResponse CompleteMultipartUpload(string bucketName, string key, string uploadId, List<PartETag> partETags)
	{
		return CompleteMultipartUpload(new CompleteMultipartUploadRequest
		{
			BucketName = bucketName,
			Key = key,
			UploadId = uploadId,
			PartETags = partETags
		});
	}

	public CompleteMultipartUploadResponse CompleteMultipartUpload(string bucketName, string key, string uploadId, List<PartETag> partETags, ObjectMetadata metadata)
	{
		return CompleteMultipartUpload(new CompleteMultipartUploadRequest
		{
			BucketName = bucketName,
			Key = key,
			UploadId = uploadId,
			PartETags = partETags,
			ObjectMetadata = metadata
		});
	}

	public CompleteMultipartUploadResponse CompleteMultipartUpload(CompleteMultipartUploadRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("POST", request);
		internalRequest.Parameters["uploadId"] = request.UploadId;
		ObjectMetadata objectMetadata = request.ObjectMetadata;
		if (objectMetadata != null)
		{
			PopulateRequestMetadata(internalRequest, objectMetadata);
		}
		string json = JsonUtils.SerializeObject(new PartETags
		{
			Parts = request.PartETags
		});
		FillRequestBodyForJson(internalRequest, json);
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				CompleteMultipartUploadResponse completeMultipartUploadResponse = ToObject<CompleteMultipartUploadResponse>(httpWebResponse);
				completeMultipartUploadResponse.BucketName = request.BucketName;
				return completeMultipartUploadResponse;
			}
		});
	}

	public void AbortMultipartUpload(string bucketName, string key, string uploadId)
	{
		AbortMultipartUpload(new AbortMultipartUploadRequest
		{
			BucketName = bucketName,
			Key = key,
			UploadId = uploadId
		});
	}

	public void AbortMultipartUpload(AbortMultipartUploadRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("DELETE", request);
		internalRequest.Parameters["uploadId"] = request.UploadId;
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BosResponseBase>(httpWebResponse);
			}
		});
	}

	public ListMultipartUploadsResponse ListMultipartUploads(string bucketName)
	{
		return ListMultipartUploads(new ListMultipartUploadsRequest
		{
			BucketName = bucketName
		});
	}

	public ListMultipartUploadsResponse ListMultipartUploads(ListMultipartUploadsRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest("GET", request);
		internalRequest.Parameters["uploads"] = null;
		if (!string.IsNullOrEmpty(request.KeyMarker))
		{
			internalRequest.Parameters["keyMarker"] = request.KeyMarker;
		}
		if (!string.IsNullOrEmpty(request.Delimiter))
		{
			internalRequest.Parameters["delimiter"] = request.Delimiter;
		}
		if (!string.IsNullOrEmpty(request.Prefix))
		{
			internalRequest.Parameters["prefix"] = request.Prefix;
		}
		if (request.MaxUploads.HasValue)
		{
			internalRequest.Parameters["maxUploads"] = request.MaxUploads.ToString();
		}
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				ListMultipartUploadsResponse listMultipartUploadsResponse = ToObject<ListMultipartUploadsResponse>(httpWebResponse);
				listMultipartUploadsResponse.BucketName = request.BucketName;
				return listMultipartUploadsResponse;
			}
		});
	}

	protected override T ToObject<T>(HttpWebResponse httpWebResponse)
	{
		T val = base.ToObject<T>(httpWebResponse);
		if (val is BosResponseBase)
		{
			(val as BosResponseBase).BosDebugId = httpWebResponse.Headers["x-bce-debug-id"];
		}
		return val;
	}

	private ObjectMetadata GetObjectMetadata(HttpWebResponse httpWebResponse)
	{
		ObjectMetadata objectMetadata = new ObjectMetadata();
		if (long.TryParse(httpWebResponse.GetResponseHeader("Content-Length"), out var result))
		{
			objectMetadata.ContentLength = result;
		}
		objectMetadata.ContentType = httpWebResponse.GetResponseHeader("Content-Type");
		objectMetadata.ContentEncoding = httpWebResponse.GetResponseHeader("Content-Encoding");
		objectMetadata.ContentMd5 = httpWebResponse.GetResponseHeader("Content-MD5");
		objectMetadata.ContentDisposition = httpWebResponse.GetResponseHeader("Content-Disposition");
		string responseHeader = httpWebResponse.GetResponseHeader("ETag");
		if (responseHeader != null)
		{
			objectMetadata.ETag = responseHeader.Replace("\"", "");
		}
		string text = (objectMetadata.ContentRange = httpWebResponse.GetResponseHeader("Content-Range"));
		if (text != null)
		{
			int num = text.LastIndexOf('/');
			if (num >= 0)
			{
				try
				{
					objectMetadata.InstanceLength = long.Parse(text.Substring(num + 1));
				}
				catch (FormatException ex)
				{
                    Logger.LogWarning(ex, ("Fail to parse length from Content-Range: " + text));
				}
			}
		}
		objectMetadata.LastModified = DateUtils.ParseRfc822Date(httpWebResponse.GetResponseHeader("Last-Modified"));
		objectMetadata.BceContentSha256 = httpWebResponse.GetResponseHeader("x-bce-content-sha256");
		string responseHeader3 = httpWebResponse.GetResponseHeader("x-bce-storage-class");
		if (!string.IsNullOrEmpty(responseHeader3))
		{
			objectMetadata.StorageClass = responseHeader3;
		}
		else
		{
			objectMetadata.StorageClass = "STANDARD";
		}
		string[] allKeys = httpWebResponse.Headers.AllKeys;
		foreach (string text2 in allKeys)
		{
			if (text2.StartsWith("x-bce-meta-"))
			{
				string str = text2.Substring("x-bce-meta-".Length);
				objectMetadata.UserMetadata[HttpUtility.UrlDecode(str)] = HttpUtility.UrlDecode(httpWebResponse.GetResponseHeader(text2));
			}
		}
		return objectMetadata;
	}

	private static void PopulateRequestMetadata(InternalRequest request, ObjectMetadata metadata)
	{
		if (metadata.ContentType != null)
		{
			request.Headers["Content-Type"] = metadata.ContentType;
		}
		if (metadata.ContentMd5 != null)
		{
			request.Headers["Content-MD5"] = metadata.ContentMd5;
		}
		if (metadata.ContentEncoding != null)
		{
			request.Headers["Content-Encoding"] = metadata.ContentEncoding;
		}
		if (metadata.ContentDisposition != null)
		{
			request.Headers["Content-Disposition"] = metadata.ContentDisposition;
		}
		if (metadata.BceContentSha256 != null)
		{
			request.Headers["x-bce-content-sha256"] = metadata.BceContentSha256;
		}
		if (metadata.ETag != null)
		{
			request.Headers["ETag"] = metadata.ETag;
		}
		if (metadata.StorageClass != null)
		{
			request.Headers["x-bce-storage-class"] = metadata.StorageClass;
		}
		IDictionary<string, string> userMetadata = metadata.UserMetadata;
		if (userMetadata == null)
		{
			return;
		}
		foreach (KeyValuePair<string, string> item in userMetadata)
		{
			string key = item.Key;
			if (key != null)
			{
				string text = item.Value;
				if (text == null)
				{
					text = "";
				}
				string key2 = "x-bce-meta-" + HttpUtils.Normalize(key.Trim());
				request.Headers[key2] = HttpUtils.Normalize(text);
			}
		}
	}

	private InternalRequest CreateInternalRequest(string httpMethod, BceRequestBase request)
	{
		string text = null;
		string text2 = null;
		if (request is BucketRequestBase)
		{
			text = (request as BucketRequestBase).BucketName;
		}
		if (request is ObjectRequestBase)
		{
			text2 = (request as ObjectRequestBase).Key;
		}
		if (request.Config != null && request.Config.Endpoint != null)
		{
			string endpoint = request.Config.Endpoint;
			if (request.Config.CnameEnabled == true || DomainUtils.IsCnameLikeHost(endpoint) || DomainUtils.IsCustomHost(endpoint, text))
			{
				return CreateInternalRequest(request, httpMethod, new string[2] { "/v1", text2 });
			}
			if (request.Config.PathStyleEnabled == false)
			{
				string text3 = DomainUtils.ConvertEndpointToVirtualHostedStyle(endpoint, text);
				if (text3 != request.Config.Endpoint)
				{
					request.Config.Endpoint = text3;
					return CreateInternalRequest(request, httpMethod, new string[2] { "/v1", text2 });
				}
			}
		}
		return CreateInternalRequest(request, httpMethod, new string[3] { "/v1", text, text2 });
	}

	private InternalRequest CreateInternalRequestForGetObject(GetObjectRequest request)
	{
		InternalRequest internalRequest = CreateInternalRequest("GET", request);
		long[] range = request.Range;
		if (range != null && range.Length == 2)
		{
			internalRequest.Range = request.Range;
		}
		if (request.TrafficLimit != 0L)
		{
			internalRequest.Headers["x-bce-traffic-limit"] = request.TrafficLimit.ToString();
		}
		return internalRequest;
	}

	private Uri ConvertRequestToUri(InternalRequest request)
	{
		string text = request.Uri.AbsolutePath;
		if (text.StartsWith("/"))
		{
			text = text.Substring(1);
		}
		string text2 = "/" + text;
		text2 = text2.Replace("(?<=/)/", "%2F");
		string text3 = ComputeEndpoint(request.Config) + text2;
		bool flag = true;
		foreach (KeyValuePair<string, string> parameter in request.Parameters)
		{
			if (flag)
			{
				text3 += "?";
				flag = false;
			}
			else
			{
				text3 += "&";
			}
			text3 = text3 + parameter.Key + "=" + HttpUtils.Normalize(parameter.Value);
		}
		string text4 = request.Headers["Authorization"];
		if (text4 != null)
		{
			text3 = ((!flag) ? (text3 + "&") : (text3 + "?"));
			text3 = text3 + "authorization=" + HttpUtils.Normalize(text4);
		}
		try
		{
			return new Uri(HttpUtility.UrlDecode(text3, Encoding.UTF8));
		}
		catch (UriFormatException ex)
		{
			throw new BceClientException("Unable to convert request to well formed URL: " + ex.Message, ex);
		}
	}
}
