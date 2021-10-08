import arrs

def conv_to_bin(to_convert):
	result = ""
	for i in range(0,len(to_convert)):
		result+=arrs.c2b_arr[to_convert[i]]
	return result

def conv_to_hex(to_convert):
	result=""
	for i in range(0,len(to_convert),4):
		temp=""
		for j in range(i,i+4):
			temp+=to_convert[j]
		result+=arrs.c2h_arr[temp]
	return result

def conv_to_dec(to_convert): 
    result=0
    for i in range(0,len(str(to_convert))):
        if to_convert>0:
            temp=to_convert%10
            result+=temp * pow(2, i) 
            to_convert//=10
        else:
            break
    return result
  
def conv_to_bin2(to_convert): 
	result=bin(to_convert)
	result=result[2:]
	while(len(str(result)) != 4):
		result="0"+result
	return result

def op_perm(to_perm, arr):
    result = ""
    for i in range(0, len(arr)):
        result+=to_perm[arr[i]-1]
    return result
  
def half_shift(original, which):
	result = original
	if which in [0,1,8,15]:
		amount=1
	else:
		amount=2
	for i in range(0, amount):
		s = ""
		for j in range(1,len(result)):
			s = s + result[j]
		s = s + result[0]
		result = s
	return result 

def op_xor(one, two):
    result = ""
    for i in range(0,len(one)):
        if one[i]==two[i]:
            result+="0"
        else:
            result+="1"
    return result

def op_perm_1(to_perm):
	return op_perm(to_perm, arrs.pc1)

def op_perm_2(to_perm):
	return op_perm(to_perm, arrs.pc2)

def encrypt(plaintext, roundkey):
	convertedText = conv_to_bin(plaintext)
	first_perm = op_perm(convertedText, arrs.first_perm)
	left = first_perm[0:32]
	right = first_perm[32:64]
	for i in range(0, 16):
		e_from_r = op_perm(right, arrs.e_table) 
		k_r = op_xor(e_from_r, roundkey[i])
		s_arrs_str = ""
		for j in range(0, len(arrs.s_arrs)):
			offset=j*6
			row=conv_to_dec(int(k_r[offset]+k_r[offset+5]))
			for k in range(1,5):
				col=""
				for l in range(1,5):
					col+=k_r[offset+l]
				col=conv_to_dec(int(col))
			dec=arrs.s_arrs[j][row][col]
			s_arrs_str+=str(conv_to_bin2(dec))  
		f_val = op_perm(s_arrs_str, arrs.p_table)
		result = op_xor(left, f_val)
		left = result
		temp = left
		left=right
		right=temp
		print(str(i)+" "+str(conv_to_hex(left))+" "+str(conv_to_hex(right)))
	temp=left
	left=right
	right=temp
	both=left+right
	result=op_perm(both, arrs.last_perm)
	return result

def round_keys(key):
	l=key[0:28]
	r=key[28:56]
	result=[]
	for i in range(0, 16):
		l = half_shift(l, i)
		r = half_shift(r, i)
		both = l + r 
		round_key = op_perm_2(both)
		result.append(round_key)
	return result

