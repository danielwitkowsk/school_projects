#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string> 
#include "math.h"
class xTS_PacketHeader
{
public:
    enum class ePID : uint16_t
    {
        PAT = 0x0000,
        CAT = 0x0001,
        TSDT = 0x0002,
        IPMT = 0x0003,
        NIT = 0x0010,
        SDT = 0x0011,
        NuLL = 0x1FFF,
    };

protected:
    int sb, e, s, t, pid, tsc, afc, cc;
    int afc_len, disc_ind, rand_acc_ind, elem_stream_prio, pcr_flag, opcr_flag, spl_flag, trans_priv_flag, af_ext_flag;
    int offset;
    long long pcr_base, pcr_result, opcr_base, opcr_result;
    int pcr_ext, opcr_ext, splice_cntdown, stuffing, priv_data_len;

public:
    void     Reset();
    void     Parse(std::vector <uint8_t> data);
    void     Print() const;
    bool     IsStarted();
    int      AF_len();
    bool     IsDone();
    int      ReadCC();
    int      ReadPID();
    int      ReadAFC();
    int     ReadAF();
    int     ReadSB();
    bool     HasAF();
};
void xTS_PacketHeader::Parse(std::vector <uint8_t> data) {
    sb = data[0];
    e = ((unsigned char)data[1] >> 7) & 1;
    s = ((unsigned char)data[1] >> 6) & 1;
    t = ((unsigned char)data[1] >> 5) & 1;
    pid = (((unsigned char)data[1] >> 4) & 1) * 4096 +
        (((unsigned char)data[1] >> 3) & 1) * 2048 +
        (((unsigned char)data[1] >> 2) & 1) * 1024 +
        (((unsigned char)data[1] >> 1) & 1) * 512 +
        (((unsigned char)data[1] >> 0) & 1) * 256 + data[2];
    tsc = (((unsigned char)data[3] >> 7) & 1) * 2 + (((unsigned char)data[3] >> 6) & 1);
    afc = (((unsigned char)data[3] >> 5) & 1) * 2 + (((unsigned char)data[3] >> 4) & 1);
    cc = (((unsigned char)data[3] >> 3) & 1) * 8 +
        (((unsigned char)data[3] >> 2) & 1) * 4 +
        (((unsigned char)data[3] >> 1) & 1) * 2 +
        (((unsigned char)data[3] >> 0) & 1);
    if (afc == 2 || afc == 3) {
        afc_len = data[4];
        disc_ind = ((unsigned char)data[5] >> 7) & 1;
        rand_acc_ind = ((unsigned char)data[5] >> 6) & 1;
        elem_stream_prio = ((unsigned char)data[5] >> 5) & 1;
        pcr_flag = ((unsigned char)data[5] >> 4) & 1;
        opcr_flag = ((unsigned char)data[5] >> 3) & 1;
        spl_flag = ((unsigned char)data[5] >> 2) & 1;
        trans_priv_flag = ((unsigned char)data[5] >> 1) & 1;
        af_ext_flag = ((unsigned char)data[5] >> 0) & 1;
        stuffing = afc_len - 1;
        offset = 6;
        if (pcr_flag == 1) {
            stuffing -= 6;
            for (int i = 32; i >= 25; i--) {
                pcr_base += (((unsigned char)data[offset] >> i - 25) & 1) * pow(2, i);
            }
            offset++;
            for (int i = 24; i >= 17; i--) {
                pcr_base += (((unsigned char)data[offset] >> i - 17) & 1) * pow(2, i);
            }
            offset++;
            for (int i = 16; i >= 9; i--) {
                pcr_base += (((unsigned char)data[offset] >> i - 9) & 1) * pow(2, i);
            }
            offset++;
            for (int i = 8; i >= 1; i--) {
                pcr_base += (((unsigned char)data[offset] >> i - 1) & 1) * pow(2, i);
            }
            offset++;
            pcr_base += ((unsigned char)data[offset] >> 7) & 1;
            pcr_ext += (((unsigned char)data[offset] >> 0) & 1) * 256;
            offset++;
            for (int i = 7; i >= 0; i--) {
                pcr_ext += (((unsigned char)data[offset] >> i) & 1) * pow(2, i);
            }
            pcr_result = pcr_base * 300 + pcr_ext;
            offset++;
        }
        if (opcr_flag) {
            stuffing -= 6;
            for (int i = 32; i >= 25; i--) {
                opcr_base += (((unsigned char)data[offset] >> i - 25) & 1) * pow(2, i);
            }
            offset++;
            for (int i = 24; i >= 17; i--) {
                opcr_base += (((unsigned char)data[offset] >> i - 17) & 1) * pow(2, i);
            }
            offset++;
            for (int i = 16; i >= 9; i--) {
                opcr_base += (((unsigned char)data[offset] >> i - 9) & 1) * pow(2, i);
            }
            offset++;
            for (int i = 8; i >= 1; i--) {
                opcr_base += (((unsigned char)data[offset] >> i - 1) & 1) * pow(2, i);
            }
            offset++;
            opcr_base += ((unsigned char)data[offset] >> 7) & 1;
            opcr_ext += (((unsigned char)data[offset] >> 0) & 1) * 256;
            offset++;
            for (int i = 7; i >= 0; i--) {
                opcr_ext += (((unsigned char)data[offset] >> i) & 1) * pow(2, i);
            }
            opcr_result = opcr_base * 300 + opcr_ext;
            offset++;
        }
        if (spl_flag) {
            stuffing -= 1;
            splice_cntdown = data[offset];
            offset++;
        }
        if (trans_priv_flag) {
            priv_data_len = data[offset];
            stuffing -= (1 + priv_data_len);
        }
        if (af_ext_flag) {
            stuffing = 0;
        }
    }

}

void xTS_PacketHeader::Print() const {
    std::cout << sb << " " << e << " " << s << " " << t << " " << pid << " " << tsc << " " << afc << " " << cc << " ";
    if (afc == 2 || afc == 3) {
        std::cout << afc_len << " " << disc_ind << " " << rand_acc_ind << " " <<
            elem_stream_prio << " " << pcr_flag << " " << opcr_flag << " " << spl_flag << " " << trans_priv_flag << " " << af_ext_flag;
        std::cout << " stuffing:" << stuffing;
    }
    if (pcr_flag) {
        std::cout << " pcr:" << pcr_result;
    }
    if (opcr_flag) {
        std::cout << " opcr:" << opcr_result;
    }

}
void xTS_PacketHeader::Reset() {
    sb = 0, e = 0, s = 0, t = 0, pid = 0, tsc = 0, afc = 0, cc = 0;
    afc_len = 0, disc_ind = 0, rand_acc_ind = 0, elem_stream_prio = 0, pcr_flag = 0, opcr_flag = 0, spl_flag = 0, trans_priv_flag = 0, af_ext_flag = 0;
    offset = 0;
    pcr_base = 0, pcr_result = 0;
    pcr_ext = 0;
}

bool xTS_PacketHeader::IsStarted() {
    if (s == 1 && rand_acc_ind == 1) return true;
    else return false;
}

int xTS_PacketHeader::AF_len() {
    return afc_len;
}

bool xTS_PacketHeader::IsDone() {
    if (s == 1) return true;
    else return false;
}

int xTS_PacketHeader::ReadCC() {
    return cc;
}

int xTS_PacketHeader::ReadPID() {
    return pid;
}

int xTS_PacketHeader::ReadAFC() {
    return afc;
}

int xTS_PacketHeader::ReadAF() {
    return afc;
}

int xTS_PacketHeader::ReadSB() {
    return sb;
}

bool xTS_PacketHeader::HasAF() {
    if (afc == 2 || afc == 3)
        return true;
    else
        return false;
}
class TheSounds {
    int streamtypes[8] = { 188,190,191,240,241,255,242,248 };
    int pscp = 0, streamid, PESPL, PESHDL;
    int sizee = 0;
    int anothersize = 0;
    int DTS_PTS_flag = 0;
    long long PTS = 0, DTS = 0;
public:
    void Reset() {
        pscp = 0;
        sizee = 0;
        anothersize = 0;
    }
    void Print() {
        std::cout << " " << sizee << " " << anothersize << " " <<  pscp << '\n';
    }
    void Parse(std::vector <uint8_t> data) {
        FILE* pFile;
        pFile = fopen("PID136.mp2", "a+b");
        anothersize += data.size();
        if (pscp == 1) {
            uint8_t* buffer = std::data(data);
            fwrite(buffer, 1, data.size(), pFile);
            sizee += data.size();
        }
        else if (pscp == 0) {
            pscp = data[0] + data[1] + data[2];
            streamid = data[3];
            PESPL = (data[4] << 8) + data[5];
            bool exists = std::find(std::begin(streamtypes), std::end(streamtypes), streamid) != std::end(streamtypes);
            if (exists == false) {
                DTS_PTS_flag = (((unsigned char)data[7] >> 7) & 1) * 2 + (((unsigned char)data[7] >> 6) & 1);
                std::cout << " dts_pts_flag:" << DTS_PTS_flag;
                if (DTS_PTS_flag == 2) {
                    PTS = data[9] & 0b00001110;
                    PTS <<= 7;
                    PTS += data[10];
                    PTS <<= 8;
                    PTS += data[11] & 0b11111110;
                    PTS <<= 7;
                    PTS += data[12];
                    PTS <<= 7;
                    PTS += data[13] >> 1;
                    std::cout << " pts:" << PTS;
                }
                else if (DTS_PTS_flag == 3) {
                    PTS = data[9] & 0b00001110;
                    PTS <<= 7;
                    PTS += data[10];
                    PTS <<= 8;
                    PTS += data[11] & 0b11111110;
                    PTS <<= 7;
                    PTS += data[12];
                    PTS <<= 7;
                    PTS += data[13] >> 1;
                    std::cout << " pts:" << PTS;
                    DTS = data[14] & 0b00001110;
                    DTS <<= 7;
                    DTS += data[15];
                    DTS <<= 8;
                    DTS += data[16] & 0b11111110;
                    DTS <<= 7;
                    DTS += data[17];
                    DTS <<= 7;
                    DTS += data[18] >> 1;
                    std::cout << " dts:" << DTS;
                }
                PESHDL = data[8];
                data.erase(data.begin(), data.begin() + 9 + PESHDL);
                uint8_t* buffer = std::data(data);
                fwrite(buffer, 1, data.size(), pFile);
                sizee += data.size();
            }
            else {
                PESHDL = data[8];
                data.erase(data.begin(), data.begin() + 9 + PESHDL);
                uint8_t* buffer = std::data(data);
                fwrite(buffer, 1, data.size(), pFile);
                sizee += data.size();
            }

        }
        fclose(pFile);
    }
};

class TheFrames {
    int streamtypes[8] = { 188,190,191,240,241,255,242,248 };
    int pscp = 0, streamid, PESPL, PESHDL;
    int sizee = 0;
    int anothersize = 0;
    int DTS_PTS_flag = 0;
    long long PTS = 0, DTS=0;
public:
    void Reset() {
        pscp = 0;
        sizee = 0;
        anothersize = 0;
    }
    void Print() {
        std::cout << " sizes:" << sizee << " " << anothersize;
    }
    void Parse(std::vector <uint8_t> data) {
        FILE* pFile;
        pFile = fopen("PID174.264", "a+b");
        anothersize += data.size();
        if (pscp == 1) {
            uint8_t* buffer = std::data(data);
            fwrite(buffer, 1, data.size(), pFile);
            sizee += data.size();
        }
        else if (pscp == 0) {
            pscp = data[0] + data[1] + data[2];
            streamid = data[3];
            PESPL = (data[4] << 8) + data[5];
            bool exists = std::find(std::begin(streamtypes), std::end(streamtypes), streamid) != std::end(streamtypes);
            if (exists == false) {
                DTS_PTS_flag = (((unsigned char)data[7] >> 7) & 1) * 2 + (((unsigned char)data[7] >> 6) & 1);
                std::cout << " dts_pts_flag:" << DTS_PTS_flag;
                if (DTS_PTS_flag == 2) {
                    PTS = data[9] & 0b00001110;
                    PTS <<= 7;
                    PTS += data[10];
                    PTS <<= 8;
                    PTS += data[11] & 0b11111110;
                    PTS <<= 7;
                    PTS += data[12];
                    PTS <<= 7;
                    PTS += data[13] >> 1;
                    std::cout << " pts:" << PTS;
                }
                else if (DTS_PTS_flag == 3) {
                    PTS = data[9] & 0b00001110;
                    PTS <<= 7;
                    PTS += data[10];
                    PTS <<= 8;
                    PTS += data[11] & 0b11111110;
                    PTS <<= 7;
                    PTS += data[12];
                    PTS <<= 7;
                    PTS += data[13] >> 1;
                    std::cout << " pts:" << PTS;
                    DTS = data[14] & 0b00001110;
                    DTS <<= 7;
                    DTS += data[15];
                    DTS <<= 8;
                    DTS += data[16] & 0b11111110;
                    DTS <<= 7;
                    DTS += data[17];
                    DTS <<= 7;
                    DTS += data[18] >> 1;
                    std::cout << " dts:" << DTS;
                }
                PESHDL = data[8];
                data.erase(data.begin(), data.begin() + 9 + PESHDL);
                uint8_t* buffer = std::data(data);
                fwrite(buffer, 1, data.size(), pFile);
                sizee += data.size();
            }
            else {
                data.erase(data.begin(), data.begin() + 6);
                uint8_t* buffer = std::data(data);
                fwrite(buffer, 1, data.size(), pFile);
                sizee += data.size();
            }

        }
        fclose(pFile);
    }
};
std::vector<uint8_t> readFile(const char* filename)
{
    std::streampos fileSize;
    std::ifstream file(filename, std::ios::binary);

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> fileData(fileSize);
    file.read((char*)&fileData[0], fileSize);
    return fileData;
}

int main(int argc, char* argv[], char* envp[])
{
    FILE* pFile;
    FILE* vFile;
    pFile = fopen("PID136.mp2", "w");
    vFile = fopen("PID174.264", "w");
    fclose(pFile);
    fclose(vFile);
    std::vector<uint8_t> fileData = readFile("example_new.ts");
    std::vector<uint8_t> soundData;
    xTS_PacketHeader    TS_PacketHeader;
    TheSounds           thedata;
    TheFrames           themoves;
    int32_t TS_PacketId = 0;
    int counter = 0;
    int state = 0;
    int justpackets = 0;
    for (int i = 0; i < fileData.size() / 188; i++)
    {
        std::cout << i << " ";
        std::vector<uint8_t>::const_iterator first = fileData.begin() + i * 188;
        std::vector<uint8_t>::const_iterator last = fileData.begin() + (i + 1) * 188;
        std::vector<uint8_t> newVec(first, last);

        TS_PacketHeader.Reset();
        TS_PacketHeader.Parse(newVec);
        TS_PacketHeader.Print();

        if (TS_PacketHeader.ReadPID() == 136) {
            if (state == 1) {
                if (counter + 1 == TS_PacketHeader.ReadCC()) {
                    if (TS_PacketHeader.HasAF()) {
                        state = 0;
                        std::vector<uint8_t>::const_iterator first = fileData.begin() + i * 188 + 5 + TS_PacketHeader.AF_len();
                        std::vector<uint8_t> newVec(first, last);
                        thedata.Parse(newVec);
                        counter = 0;
                        thedata.Reset();
                        //thedata.Print();
                    }
                    else {
                        std::vector<uint8_t>::const_iterator first = fileData.begin() + i * 188 + 4;
                        std::vector<uint8_t> newVec(first, last);
                        thedata.Parse(newVec);
                        counter++;
                        //thedata.Print();
                    }
                }
            }
            else if (TS_PacketHeader.IsStarted() && state == 0) {
                state = 1;
                std::vector<uint8_t>::const_iterator first = fileData.begin() + i * 188 + 5 + TS_PacketHeader.AF_len();
                std::vector<uint8_t> newVec(first, last);
                thedata.Parse(newVec);
                counter = TS_PacketHeader.ReadCC();
                //thedata.Print();
                std::cout << " start";
            }

        }
        else if (TS_PacketHeader.ReadPID() == 174) {
            if (TS_PacketHeader.IsDone()) {
                themoves.Print();
                themoves.Reset();
                justpackets++;
                if (justpackets == 2000)
                    break;
            }
            if (TS_PacketHeader.ReadAFC() == 1) {
                std::vector<uint8_t>::const_iterator first = fileData.begin() + i * 188 + 4;
                std::vector<uint8_t> newVec(first, last);
                themoves.Parse(newVec);
            }
            else if (TS_PacketHeader.ReadAFC() == 3) {
                std::vector<uint8_t>::const_iterator first = fileData.begin() + i * 188 + 5 + TS_PacketHeader.AF_len();
                std::vector<uint8_t> newVec(first, last);
                themoves.Parse(newVec);
            }
        }
        TS_PacketId++;
        std::cout << "\n";
    }

}