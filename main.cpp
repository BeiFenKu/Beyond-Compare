#include<stdio.h>
#include<direct.h>
#include<io.h>
#include<string.h>
#include<iostream>
#include <windows.h>

using namespace std;

int compareArray(unsigned char *array1, int array1Size, unsigned char *array2, int array2Size);
int modifyPE(FILE *fp, unsigned char *oriData, int oriDataSize, unsigned char *newData, int newDataSize);

void main() {


	/**
	 * (�ı�ʹ��Ȩ��)Ҫ�滻��ָ��
	 * movzx rax, byte ptr ds:[rcx+0x600]
	 * ret
	 */
	unsigned char oriData[] = {0x48, 0x0F, 0xB6, 0x81, 0x00,
		0x06, 0x00, 0x00, 0xC3, 0xCC,
		0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
		0xCC};

	/**
	 * (�ı�ʹ��Ȩ��)�µ�ָ��
	 * add rcx, 0x600
	 * mov word ptr ds:[ecx], 0x500
	 * xor eax,eax
	 * ret
	 */
	unsigned char crackData[] = {0x48, 0x81, 0xC1, 0x00, 0x06,
		0x00, 0x00, 0x67, 0x66, 0xC7,
		0x01, 0x00, 0x05, 0x33, 0xC0,
		0xC3};

	/**
	 * (ȥ����ʾ��)Ҫ�滻��ָ��
	 **/
    unsigned char oriJmpData[] = {0x06, 0x00, 0x00, 0x00, 0x0F,
		0x84, 0x3C, 0x01, 0x00, 0x00, 
	    0x48, 0x8B, 0x4D, 0x30, 0xE8};

	/**
	 * (ȥ����ʾ��)�µ�ָ��
	 **/
    unsigned char crackJmpData[] = {0x06, 0x00, 0x00, 0x00, 0xE9,
		0x3D, 0x01, 0x00, 0x00, 0x90,
	0x48, 0x8B, 0x4D, 0x30, 0xE8};

	/**
	 * ��ȡ��ǰ����Ŀ¼
	 */
	char path[80];
	getcwd(path, sizeof(path));

	/**
	 * ���ҵ�ǰ����Զ�е�Ŀ¼���Ƿ����BCompare.exe�ļ�
	 */
	struct _finddata_t data;
	long hnd = _findfirst("BCompare.exe", &data);
	int nRet = (hnd < 0) ? -1 : 1;
	if(nRet >= 0) {
		printf("�ҵ�BCompare.exe����ʼ�ƽ�... \n");
		strcat(path, "\\");
		strcat(path, data.name);
		FILE *fp;
		if((fp = fopen(path, "rb+")) == NULL) {
		    printf("file cannot open \n");
			exit(0);
		} 

        printf("��ʼ��ѯȨ�޴��룬���Ժ�...");
		int mRes1 = modifyPE(fp, oriData, sizeof(oriData), crackData, sizeof(crackData));
        printf("��ʼȥ����ʾ�����Ժ�...");
        int mRes2 = modifyPE(fp, oriJmpData, sizeof(oriJmpData), crackJmpData, sizeof(crackJmpData));


		if(mRes1 == 0 && mRes2 == 0) {
            printf("�ƽ�ɹ� \n");
		} else {
		    printf("�ƽ�ʧ�� \n");
		}
		fclose(fp);
	} else {
	    printf("û���ҵ�BCompare.exe, �뽫���򿽱���BCompare.exeĿ¼�� \n");
	}
	_findclose(hnd);//�رյ�ǰ���
	system("pause");
}


/**
 *
 * �Ƚ��ַ������Ƿ����
 */
int compareArray(unsigned char *array1, int array1Size, unsigned char *array2, int array2Size) {
	if(array1Size != array2Size) {
	    return -1;
	}
	int i;
	for(i = 0; i < array1Size; i ++) {
		if(array1[i] != array2[i]) {
		    return -1;
		}
	}
	return 0;
}

/**
 *
 * ��ѯPEһ�ζ��������ݲ��޸ĳ���һ�ζ���������
 */
int modifyPE(FILE *fp, unsigned char *oriData, int oriDataSize, unsigned char *newData, int newDataSize) {
        unsigned char *curReadData = (unsigned char *)malloc(oriDataSize);
		fseek(fp, 0, SEEK_SET);
		int rNum = fread(&curReadData[0], sizeof(char), oriDataSize, fp);
		while(rNum > 0) {
		    int cRes = compareArray(curReadData, rNum, oriData, oriDataSize);
			if(cRes == 0) {
				printf("�Ѿ��ҵ����޸�λ�� \n");

			    //���������ȣ����ļ�λ��ָʾ�����õ��ϴζ�ȡ��λ��
				fseek(fp, -rNum, SEEK_CUR);
				
				//д��������
                fwrite(&newData[0], sizeof(char), newDataSize, fp);

				free(curReadData);
				return 0;
			} else {
				//������ݲ���ȣ����ļ�λ��ָʾ�����õ��ϴζ�ȡ��λ��+1�ֽڣ���������ȡarraySize���ֽڵ�����
                fseek(fp, -rNum + 1, SEEK_CUR);
			    rNum = fread(&curReadData[0], sizeof(char), oriDataSize, fp);
			}
		}
		free(curReadData);
        printf("δ�ҵ����޸�λ�� \n");
		return -1;
}