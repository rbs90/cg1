#ifndef CG_FRAMEBUFFER_H
#define CG_FRAMEBUFFER_H

//---------------------------------------------------------------------------

class CGFrameBuffer
{
public:
	bool allocate(int width, int height);
	int getWidth() const;
	int getHeight() const;
	class CGBuffer4UB
	{
	public:
		CGBuffer4UB();
		~CGBuffer4UB();
		bool reserve(int width, int height);
		void get(int x, int y, float *rgba) const;
		void set(int x, int y, const float *rgba);
		void clear(const float *rgba);
		unsigned char* getDataPointer() const;
		int getWidth() const;
		int getHeight() const;
	private:
		int width, height;
		unsigned char* pBufferData;
	} colorBuffer;

	class CGBuffer1f
	{
	public:
		CGBuffer1f();
		~CGBuffer1f();
		bool reserve(int width, int height);
		float get(int x, int y) const;
		void set(int x, int y, const float z);
		void clear();
		float* getDataPointer() const;
		int getWidth() const;
		int getHeight() const;
	private:
		int width, height;
		float* pBufferData;
	} depthBuffer;
};

//---------------------------------------------------------------------------
#endif
