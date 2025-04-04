#include "textureclass.h"

TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// �̹����� ���� ���� ũ��� �̹��� ����

	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}

	// �ؽ�ó ��ũ���� ����
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0; 
	textureDesc.ArraySize = 1;							// �ؽ�ó �迭�� ��
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 32��Ʈ RGBA
	textureDesc.SampleDesc.Count = 1;					// ��Ƽ ���ø� x
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;			// Default: GPU���� �а� ���� (�Ϲ����� ��Ȳ�� ����)
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;  // �ؽ�ó�� ���ε� �뵵 ���� 
																					// (SHADER_RESOURCE: ���̴����� ���� �� ����)
																					// (RENDER_TARGET: ���� Ÿ�����ε� ����� �� ����)
	textureDesc.CPUAccessFlags = 0;						// CPU ���� �Ұ�
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;	// MipMap ���� ���� �÷���

	// 2D �ؽ�ó ���� (�����ʹ� mipmap ������ ���� ���߿� �ø�)
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// ���� �� ������
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// m_texture�� mipmap 0Lv�� m_targaData�� ä��� (���� ���� rowPitch)
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// srv ����
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

	// ���ҽ� ����
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
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

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

// �̹����� ����, ���� ũ��� ������ ����
bool TextureClass::LoadTarga32Bit(char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// ���� ����
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// ���� ��� �б�
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// ���� ���� ����
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// �̹��� ũ��
	imageSize = m_width * m_height * 4;

	// �޸� ���� ���� �Ҵ�
	targaImage = new unsigned char[imageSize];

	// �̹��� ������ read
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// ���� �ݱ�
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (m_width * m_height * 4) - (m_width * 4);

	// ����� ������ ������ m_targaData�� ����
	for (j = 0; j < m_height; j++)
	{
		for (i = 0; i < m_width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (m_width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return true;
}


int TextureClass::GetWidth()
{
	return m_width;
}


int TextureClass::GetHeight()
{
	return m_height;
}