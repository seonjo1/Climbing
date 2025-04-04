#include "texture.h"

Texture::Texture()
{
	m_texture = 0;
	m_textureView = 0;
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, aiMaterial* materialInfo, std::string& dirname)
{
	if (materialInfo->GetTextureCount(aiTextureType_DIFFUSE) <= 0) {
		m_texture = 0;
		m_textureView = 0;
	}
	else {
		aiString filepath;
		materialInfo->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);
		std::filesystem::path fullPath = std::filesystem::path(dirname) / filepath.C_Str();
		Initialize(device, deviceContext, fullPath.string());
	}
}


Texture::Texture(const Texture& other)
{
}


Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string filename)
{
	std::wstring filepath = ConvertToWString(filename.c_str());

	TexMetadata metadata = {};
	ScratchImage image;

	HRESULT result = LoadFromWICFile(filepath.c_str(), WIC_FLAGS_FORCE_RGB,	&metadata, image);
	if (FAILED(result))
	{
		return false;
	}

	m_height = static_cast<int>(metadata.height);
	m_width = static_cast<int>(metadata.width);

	// �ؽ�ó ��ũ���� ����
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0; 
	textureDesc.ArraySize = metadata.arraySize;							// �ؽ�ó �迭�� ��
	textureDesc.Format = metadata.format;	// 32��Ʈ RGBA
	textureDesc.SampleDesc.Count = 1;					// ��Ƽ ���ø� x
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;			// Default: GPU���� �а� ���� (�Ϲ����� ��Ȳ�� ����)
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;  // �ؽ�ó�� ���ε� �뵵 ���� 
																					// (SHADER_RESOURCE: ���̴����� ���� �� ����)
																					// (RENDER_TARGET: ���� Ÿ�����ε� ����� �� ����)
	textureDesc.CPUAccessFlags = 0;						// CPU ���� �Ұ�
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;	// MipMap ���� ���� �÷���

	// 2D �ؽ�ó ���� (�����ʹ� mipmap ������ ���� ���߿� �ø�)
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// ���� �� ������
	const Image* img = image.GetImage(0, 0, 0);

	// m_texture�� mipmap 0Lv�� m_targaData�� ä��� (���� ���� rowPitch)
	deviceContext->UpdateSubresource(m_texture, 0, NULL, img->pixels, img->rowPitch, 0);

	// srv ����
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;	// mip Level 0���� ���
	srvDesc.Texture2D.MipLevels = -1;		// MipLevels = -1 : ��ü MIP ������ SRV���� �ڵ����� �ν��ؼ� ���ڴ�.

	// Shader Resource View (SRV) ����
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// MIPMAP �ڵ� ����
	deviceContext->GenerateMips(m_textureView);

	return true;
}

void Texture::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_textureView;
}

int Texture::GetWidth()
{
	return m_width;
}


int Texture::GetHeight()
{
	return m_height;
}