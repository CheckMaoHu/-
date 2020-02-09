#include "Game.h"
#include <memory>
#include <iostream>
#include <ctime> 
void Game::Chang_UpddateLimit(int &left_temp, int& right_temp, int &top_temp, int &bottom_temp, int X, int Y) {

	int MM = 3;
	if (X >= top_temp + MM && X <= bottom_temp - MM &&
		Y >= left_temp + MM && Y <= right_temp - MM)
		return;
	else if (X >= top_temp + MM && X <= bottom_temp - MM)
	{
		if (Y > right_temp - MM)
			right_temp = min(SIZE - 1, Y + MM);
		else if (Y < left_temp + MM)
			left_temp = max(0, Y - MM);
	}
	else if (Y >= left_temp + MM && Y <= right_temp - MM)
	{
		if (X > bottom_temp - MM)
			bottom_temp = min(SIZE - 1, X + MM);
		else if (X < top_temp + MM)
			top_temp = max(0, X - MM);
	}
	else
	{
		if (X < top_temp + MM && Y > right_temp - MM)
		{
			right_temp = min(SIZE - 1, Y + MM);
			top_temp = max(0, X - MM);
		}
		else if (X < top_temp + MM && Y < left_temp + MM)
		{
			left_temp = max(0, Y - MM);
			top_temp = max(0, X - MM);
		}
		else if (X > bottom_temp - MM && Y < left_temp + MM)
		{
			left_temp = max(0, Y - MM);
			bottom_temp = min(SIZE - 1, X + MM);
		}
		else if (X > bottom_temp - MM && Y > right_temp - MM)
		{
			right_temp = min(SIZE - 1, Y + MM);
			bottom_temp = min(SIZE - 1, X + MM);
		}
	}
}
bool Game::VCAttack()
{
	// �ж��Ƿ��п��Խ��� VC �����ĵ�
	// ���жϼ�����ܷ� VCF����ȡʤ�����ж����Ƿ�ͨ��VCF����ȡʤ��Ȼ�����жϼ�����Ƿ�ͨ��VCF��VCT����
	//����ȡʤ��Ҳ�ж��˵�VCF��VCT���Ϲ���(��Ȼ��һ��).
	// Ϊ����� ������ ���ϵĳ��Ը�����ȣ��ȴ�С����ȿ�ʼ���أ��������ܱ�֤����С�Ĳ���Ӯ�壬
	// һ���ҵ���Ӯ�㣬��������
	// Ŀǰ��Ȼ��bug���������ٶȺ�����֮��Ȩ��

	// VC ��������4����4(��4������4)����3����3����3�������ȼ����ν���

	int row, col;
	VCPoint vcPoint[100]; //
	int cnt;
	int tempDepth; //��ȿ���

	VCPoint dePoint[100]; //���� VC ���أ��������ģ���˵Ĺ����б��湥����
	int num = 0; //

	int tempX, tempY, depthRecord = 9999, maxScore = -9999;
	bool ff = false;
	int left1 = left, right1 = right, bottom1 = bottom, top1 = top;
	// ���жϼ�����Ƿ�ɽ��� VCF ����,  �����򷵻�

	cnt = 0;
	message.cnt = cnt;

	message.type = 1;
	for (row = top; row <= bottom; ++row)
	{
		for (col = left; col <= right; ++col)
		{
			if (chess[row][col].flag != SPACE)
				continue;
			if (BLACK_CHESS == cComputer && IsKinjite(row, col)) // ������Ľ��ֵ�
				continue;

			int res = IsCombForVC(row, col, cComputer);

			if (res<3) // VCF����
				continue;

			vcPoint[cnt++].init(row, col, res);	//�ǽ��������ȱ���

			if (cnt >= 100)
			{
				row = bottom + 1;
				col = right + 1;
			}
		}
	}

	sort(vcPoint, vcPoint + cnt); //
	message.cnt = cnt;
	for (tempDepth = VCDEPTH; tempDepth <= VCDEPTH_END; tempDepth += VCDEPTH_DIS)//��Ȳ��ϵ���
	{
		depthMM = tempDepth; //
		for (int i = 0; i<cnt; ++i)
		{
			bool flag = false;
			int ansDepth = 9999;
			VCAttackTree(1, vcPoint[i].row, vcPoint[i].col, cComputer, cPeople, 0, flag, ansDepth, depthRecord, left1, right1, top1, bottom1);

			if (flag) // �ҵ�һ����Ӯ������
			{
				ff = true; //
				if (ansDepth<depthRecord) //ѡȡ�����Ӯ��
				{
					depthRecord = ansDepth;
					tempX = vcPoint[i].row;
					tempY = vcPoint[i].col;
					maxScore = GetScoreForComputer(tempX, tempY, cComputer);
					maxScore += GetScoreForComputer(tempX, tempY, cPeople);
				}
				else if (ansDepth == depthRecord) //���һ��ʱѡȡȨֵ���
				{
					int res = GetScoreForComputer(vcPoint[i].row, vcPoint[i].col, cComputer);
					res += GetScoreForComputer(vcPoint[i].row, vcPoint[i].col, cPeople);
					if (res > maxScore)
					{
						maxScore = res;
						tempX = vcPoint[i].row;
						tempY = vcPoint[i].col;
					}
				}
			}
		}
		if (ff)
		{
			comX = tempX;
			comY = tempY;
			AIState = 1; //
			message.type = 0;
			return true;
		}

	}


	message.type = 2;
	// ���ж����Ƿ�ͨ�� VCF����ȡʤ�������ԣ������������� VCF��VCT�����Ϲ�������Ϊ������
	// ����Ҫ�����ж� ���Ƿ񻹿��� ͨ�� VCF��VCT�����Ϲ���ȡʤ�����ϵ�������������
	int peoIsVCF = false;
	int j;
	//////////////////////////////////////////////////////////////////////////
	cnt = 0;
	message.cnt = cnt;
	for (row = top; row <= bottom; ++row)
	{
		for (col = left; col <= right; ++col)
		{
			if (chess[row][col].flag != SPACE)
				continue;
			if (BLACK_CHESS == cPeople && IsKinjite(row, col)) // �˵Ľ��ֵ�
				continue;

			int res = IsCombForVC(row, col, cPeople);

			if (res<3) // VCF����
				continue;

			vcPoint[cnt++].init(row, col, res);	//�ǽ��������ȱ���
			if (cnt >= 100)
			{
				row = bottom + 1;
				col = right + 1;
			}
		}
	}
	message.cnt = cnt;
	sort(vcPoint, vcPoint + cnt); //


	for (tempDepth = VCDE_DEPTH; tempDepth <= VCDE_DEPTH_END; tempDepth += VCDE_DEPTH_DIS)//��Ȳ��ϵ���
	{
		depthMM = tempDepth; //
		for (int i = 0; i<cnt; ++i)
		{
			bool flag = false;
			int ansDepth = 9999;

			VCAttackTree(1, vcPoint[i].row, vcPoint[i].col, cPeople, cComputer, 0, flag, ansDepth, depthRecord, left1, right1, top1, bottom1);

			if (flag) // �ҵ�һ����Ӯ������
			{
				ff = true; //
				j = i;
				i = cnt;
				break;
				tempDepth = VCDE_DEPTH_END + 1;
			}
		}
	}
	if (ff)
	{
		comX = vcPoint[j].row;
		comY = vcPoint[j].col;
		peoIsVCF = true;
		AIState = 2;


	}

	ff = false;


	message.type = 4;
	//�жϼ�����ܷ�ͨ�� VCF��VCT���Ϲ���ȡʤ,  ǰ�����˲��ܽ��� VCF����
	//////////////////////////////////////////////////////////////////////////
	//if (!peoIsVCF)
	//{
	//	cnt = 0;
	//	message.cnt = cnt;
	//	for (row = top; row <= bottom; ++row)
	//	{
	//		for (col = left; col <= right; ++col)
	//		{
	//			if (chess[row][col].flag != SPACE)
	//				continue;
	//			if (BLACK_CHESS == cComputer && IsKinjite(row, col)) // ������Ľ��ֵ�
	//				continue;

	//			int res = IsCombForVC(row, col, cComputer);

	//			if (!res) // VCF��VCT���Ϲ���
	//				continue;

	//			vcPoint[cnt++].init(row, col, res);	//�ǽ��������ȱ���

	//			if (cnt >= 100)
	//			{
	//				row = bottom + 1;
	//				col = right + 1;
	//			}
	//		}
	//	}
	//	sort(vcPoint, vcPoint + cnt); //
	//	message.cnt = cnt;
	//	for (tempDepth = VCDEPTH; tempDepth <= VCDEPTH_END; tempDepth += VCDEPTH_DIS)//��Ȳ��ϵ���
	//	{
	//		depthMM = tempDepth; //
	//		for (int i = 0; i<cnt; ++i)
	//		{
	//			bool flag = false;
	//			int ansDepth = 9999;
	//			VCAttackTree(2, vcPoint[i].row, vcPoint[i].col, cComputer, cPeople, 0, flag, ansDepth, depthRecord, left1, right1, top1, bottom1);

	//			if (flag) //�ҵ�һ����Ӯ������
	//			{
	//				ff = true; //
	//				if (ansDepth<depthRecord) //ѡȡ�����Ӯ��
	//				{
	//					depthRecord = ansDepth;
	//					tempX = vcPoint[i].row;
	//					tempY = vcPoint[i].col;
	//					maxScore = GetScoreForComputer(tempX, tempY, cComputer);
	//					maxScore += GetScoreForComputer(tempX, tempY, cPeople);
	//				}
	//				else if (ansDepth == depthRecord) //���һ��ʱѡȡȨֵ���
	//				{
	//					int res = GetScoreForComputer(vcPoint[i].row, vcPoint[i].col, cComputer);
	//					res += GetScoreForComputer(vcPoint[i].row, vcPoint[i].col, cPeople);
	//					if (res > maxScore)
	//					{
	//						maxScore = res;
	//						tempX = vcPoint[i].row;
	//						tempY = vcPoint[i].col;
	//					}
	//				}

	//			}
	//		}
	//		if (ff)
	//		{
	//			comX = tempX;
	//			comY = tempY;
	//			AIState = 3;
	//			message.type = 0;
	//			return true;
	//		}
	//	}
	//}


	message.type = 3;
	//�ж����ܷ�ͨ�� VCF��VCT���Ϲ���ȡʤ��  VC ����
	//////////////////////////////////////////////////////////////////////////
	//cnt = 0;
	//depthRecord = 9999;
	//message.cnt = cnt;
	//for (row = top; row <= bottom; ++row)
	//{
	//	for (col = left; col <= right; ++col)
	//	{
	//		if (chess[row][col].flag != SPACE)
	//			continue;
	//		if (BLACK_CHESS == cPeople && IsKinjite(row, col)) // �Լ��Ľ��ֵ�
	//			continue;

	//		int res = IsCombForVC(row, col, cPeople);

	//		if (!res) // VCF��VCT���Ϲ���
	//			continue;

	//		vcPoint[cnt++].init(row, col, res);	//�ǽ��������ȱ���

	//		if (cnt >= 100)
	//		{
	//			row = bottom + 1;
	//			col = right + 1;
	//		}
	//	}
	//}

	//sort(vcPoint, vcPoint + cnt); //
	//message.cnt = cnt;
	//for (tempDepth = VCDE_DEPTH; tempDepth <= VCDE_DEPTH_END; tempDepth += VCDE_DEPTH_DIS)//��Ȳ��ϵ���
	//{
	//	depthMM = tempDepth; //
	//	for (int i = 0; i<cnt; ++i)
	//	{
	//		bool flag = false;
	//		int ansDepth = 9999;
	//		isDefend = false;

	//		VCAttackTree(2, vcPoint[i].row, vcPoint[i].col, cPeople, cComputer, 0, flag, ansDepth, depthRecord, left1, right1, top1, bottom1);

	//		if (flag) // �ҵ�һ����Ӯ������
	//		{
	//			isDefend = true; //���أ���Ҫ��Ϊ�� ���ڶԷ������ķ���
	//			ff = true; //		
	//			if (ansDepth<depthRecord) //ѡȡ�����Ӯ��
	//			{
	//				depthRecord = ansDepth;
	//				tempX = vcPoint[i].row;
	//				tempY = vcPoint[i].col;
	//				maxScore = GetScoreForComputer(tempX, tempY, cPeople);
	//				maxScore += GetScoreForComputer(tempX, tempY, cComputer);
	//			}
	//			else if (ansDepth == depthRecord) //���һ��ʱѡȡȨֵ���
	//			{
	//				int res = GetScoreForComputer(vcPoint[i].row, vcPoint[i].col, cPeople);
	//				res += GetScoreForComputer(vcPoint[i].row, vcPoint[i].col, cComputer);
	//				if (res > maxScore)
	//				{
	//					maxScore = res;
	//					tempX = vcPoint[i].row;
	//					tempY = vcPoint[i].col;
	//				}
	//			}
	//		}
	//	}
	//	if (ff)
	//	{
	//		// ���أ������ѡ�񹥻�λ�����ӣ���������Ϊ���˸õ��ʹ�˲������ VC ������
	//		//////////////////////////////////////////////////////////////////////////
	//		message.cnt = num;
	//		for (row = top; row <= bottom; ++row)
	//		{
	//			for (col = left; col <= right; ++col)
	//			{
	//				if (chess[row][col].flag != SPACE)
	//					continue;
	//				if (BLACK_CHESS == cComputer && IsKinjite(row, col))
	//					continue;

	//				int res1 = IsCombForVC(row, col, cComputer);
	//				int res2 = IsCombForVC(row, col, cPeople);

	//				if (!res1 && !res2)
	//					continue;
	//				else if ((3 == res1 || 4 == res1) && !res2) //һ������²�Ҫ���Լ���4������4
	//					continue;

	//				int res = GetScoreForComputer(row, col, cComputer);
	//				res += GetScoreForComputer(row, col, cPeople);

	//				dePoint[num++].init(row, col, res);
	//			}
	//		}

	//		sort(dePoint, dePoint + num); //��Ȩֵ����
	//		message.cnt = num;
	//		for (int p = 0; p<num; ++p)
	//		{
	//			bool noVC = true;

	//			chess[dePoint[p].row][dePoint[p].col].flag = cComputer; //����������������

	//																	// ����������ģ�� �˵� VC ����
	//			cnt = 0;          
	//			depthRecord = 9999;
	//			message.cnt = cnt;
	//			for (row = top; row <= bottom; ++row)
	//			{
	//				for (col = left; col <= right; ++col)
	//				{
	//					if (chess[row][col].flag != SPACE)
	//						continue;
	//					if (BLACK_CHESS == cPeople && IsKinjite(row, col)) // �˵Ľ��ֵ�
	//						continue;

	//					int res = IsCombForVC(row, col, cPeople);

	//					if (!res) // VCF��VCT���Ϲ���
	//						continue;

	//					vcPoint[cnt++].init(row, col, res);	//�ǽ��������ȱ���

	//					if (cnt >= 100)
	//					{
	//						row = bottom + 1;
	//						col = right + 1;
	//					}
	//				}
	//			}

	//			sort(vcPoint, vcPoint + cnt); //
	//			message.cnt = cnt;

	//			for (tempDepth = VCDE_DEPTH; tempDepth <= VCDE_DEPTH_END; tempDepth += VCDE_DEPTH_DIS)//��Ȳ��ϵ���
	//			{
	//				depthMM = tempDepth; //
	//				for (int i = 0; i<cnt; ++i)
	//				{
	//					bool flag = false;
	//					int ansDepth = 9999;
	//					VCAttackTree(2, vcPoint[i].row, vcPoint[i].col, cPeople, cComputer, 0, flag, ansDepth, depthRecord, left1, right1, top1, bottom1);

	//					if (flag) // �ҵ�����������
	//					{
	//						noVC = false;
	//						i = cnt;
	//						tempDepth = VCDE_DEPTH_END + 1;
	//					}
	//				}
	//			}

	//			chess[dePoint[p].row][dePoint[p].col].flag = SPACE; //�ָ�

	//			if (noVC) //�����û�� VC ������
	//			{
	//				tempX = dePoint[p].row;
	//				tempY = dePoint[p].col;
	//				break;
	//			}
	//		}

	//		//////////////////////////////////////////////////////////////////////////
	//		comX = tempX;
	//		comY = tempY;
	//		AIState = 4;
	//		message.type = 0;
	//		return true;
	//	}
	//}
	//message.type = 0;
	return false;
}
void Game::VCAttackTree(int type, int row, int col, char cOneself, char cOpposite, int depth, bool& flag, int& ansDepth, int depthRecord, int left1, int right1, int top1, int bottom1)
{
	message.depth = depth;
	// ��ȿ���,���̫��Ļ������ӶȾ�̫���ˣ�depthRecord������¼�Ѿ��ѹ��ĵ��д��Ӯ�����С���
	if (depth>depthMM || depth>depthRecord)
		return;

	if (cOneself == cComputer)
	{
		ComputerPlay();
		if (1 == winComputer)
			flag = true;
	}
	else
	{
		PeoplePlay();
		if (1 == winPeople)
			flag = true;
	}
	if (flag) // flag Ϊ true ʱ����ʾ���Ա�Ӯ���ܱ�Ӯ�򷵻أ�����Ҫȥ�����ŵģ�����̫��
	{
		ansDepth = depth; //��¼���ٲ���Ӯ
		return;
	}
	chess[row][col].flag = cOneself; // �Լ�����

	int oppositeX, oppositeY;
	int winOpposite = 0;
	int dd = 1; // ��ȵĵ���ֵ

				//////////////////////�Է�˼��/////////////////////////////////////////////
	if (cOpposite == cPeople)
	{
		PeoplePlay();
		oppositeX = peoX, oppositeY = peoY;
		winOpposite = winPeople;
		
	}
	else //
	{
		ComputerPlay();
		oppositeX = comX, oppositeY = comY;
		winOpposite = winComputer;
	}

	// ����Է��ڷ��ص�ͬʱ Ҳ�����˳�4���3����ô�Լ�����Ҫ���������ضԷ�����ʱӦ�ý���ȼ�1
	if (1 == type) // VCF ����
	{
		pair<int, pair<int, int> > ppi = GetChong3Chong4(oppositeX, oppositeY, cOpposite);
		if (ppi.second.first + ppi.second.second) //���ɳ�4������4
			dd = 2;

		//		if(ppi.second.first + ppi.second.second >=2 ) //
		//			winOpposite = 1;
	}
	else if (2 == type) // VCF �� VCT ���Ϲ���
	{
		pair<int, pair<int, int> > ppi = GetChong3Chong4(oppositeX, oppositeY, cOpposite);
		if (ppi.second.first + ppi.second.second) //���ɳ�4������4
			dd = 2;
		pair<int, int> pi1 = GetLian2Lian3(oppositeX, oppositeY, cOpposite);
		if (pi1.second) //������3
			dd = 2;
		pair<int, int> pi2 = GetTiao2Tiao3(oppositeX, oppositeY, cOpposite);
		if (pi2.second) //������3
			dd = 2;

		//		if(ppi.second.first+ppi.second.second + pi1.second+pi2.second >=2 ) //
		//			winOpposite = 1;
	}
	//////////////////////////////////////////////////////////////////////////
	Chang_UpddateLimit(left1, right1, top1, bottom1, oppositeX, oppositeY);
	int left_temp = left1, right_temp = right1, top_temp = top1, bottom_temp = bottom1;
	if ((oppositeX != -1 && oppositeY != -1) && (winOpposite != 1)) // ����δ�� �� �Է� δ����
	{
		if (cOpposite == BLACK_CHESS && IsKinjite(oppositeX, oppositeY)) // �õ��ǶԷ��Ľ���
			flag = true;
		else
		{
			chess[oppositeX][oppositeY].flag = cOpposite; // �Է�����

			VCPoint vcPoint[100];// �����3����4����4��Щ��
			int cnt = 0;
			int i, j;

			for (i = top_temp; i <bottom_temp; ++i)
			{
				for (j = left_temp; j <right_temp; ++j)
				{
					if (chess[i][j].flag != SPACE)
						continue;
					if (BLACK_CHESS == cOneself && IsKinjite(i, j)) // �Լ��Ľ��ֵ�
						continue;

					int res = IsCombForVC(i, j, cOneself);
					//�ǹ������򱣴�,�������ͷ�2�֣�VCF����VCF��VCT���Ϲ���
					if (1 == type && res<3)
						continue;
					if (2 == type && !res)
						continue;

					vcPoint[cnt++].init(i, j, res);
					if (cnt >= 100)
					{
						i = j = bottom_temp;
					}
				}
			}

			sort(vcPoint, vcPoint + cnt);  //�����ȼ�����

			for (i = 0; i<cnt; ++i)
			{
				if (!flag)
				{
					VCAttackTree(type, vcPoint[i].row, vcPoint[i].col, cOneself, cOpposite, depth + dd, flag, ansDepth, depthRecord, left_temp, right_temp, top_temp, bottom_temp); //
				}
			}

			chess[oppositeX][oppositeY].flag = SPACE; // �ָ�
		}
	}
	chess[row][col].flag = SPACE; //�ָ�
}
int Game::IsCombForVC(int row, int col, char chessFlag)
{
	//�ж��Ƿ��� VC �����Ĺ����㣬���������ȼ�
	Search(row, col, chessFlag);

	if (huo4)
		return 5;
	if (chong4)
		return 4;
	if (tiaochong4)
		return 3;
	if (lian3)
		return 2;
	if (tiao3)
		return 1;

	return 0;
}
//bool Game::MaxMin_Vc(int depth)
//{
//	int j = 101;
//	bool val;
//	/*VCPoint*vcpoint;
//	vcpoint = new VCPoint[100]();
//	*/
//	VCPoint vcpointoneself[100];
//	VCPoint vcpointall[100];
//	VCPoint vcpointoppsite[100];
//	int temp = 999;//Ҫ�����ֵ���Լ������Ȩ�أ����Լ�Ȩ��Ϊ׼
//	for (int i = 0; i < 100; i++)
//	{
//		vcpointoneself[i].row = 0;
//		vcpointoneself[i].col = 0;
//		vcpointoneself[i].pri = 0;
//
//		vcpointall[i].row = 0;
//		vcpointall[i].col = 0;
//		vcpointall[i].pri = 0;
//
//		vcpointoppsite[i].row = 0;
//		vcpointoppsite[i].col = 0;
//		vcpointoppsite[i].pri = 0;
//	}
//	int num = 0;
//	int num1 = FindVcOneselfPoint(left, right, top, bottom, cComputer, vcpointoneself);//������Ϊ�Լ�
//	num = FindVcPoint(left, right, top, bottom, cComputer, cPeople, vcpointall);//����ʹ�õ�
//	int num2 = FindVcOneselfPoint(left, right, top, bottom, cPeople, vcpointoppsite);//����˵��Ϊ������
//	for (int i = 0; i < num; i++)
//	{
//		
//		for (int k = 0; k < num1; k++)
//		{
//			if ((vcpointall[i].row == vcpointoneself[k].row)&&(vcpointoneself[k].col == vcpointall[i].col))
//			{
//				temp = k;
//				break;
//			}
//		}
//		if (temp != 999)
//		{
//			chess[vcpointall[i].row][vcpointall[i].col].flag = cComputer;
//			val = Min_Vc(left, right, top, bottom, vcpointall[i].row, vcpointall[i].col, depth - 1, cComputer, cPeople, vcpointall[0].pri, vcpointoneself[temp].pri);
//			//vcpointall[0].pri��Ҫ�����ڴ�����һ�ε������ֵ
//			chess[vcpointall[i].row][vcpointall[i].col].flag = SPACE;
//			if (val)
//			{
//				j = i;
//				break;//������һ����û��һ������Զ�ס�����Խض�
//			}
//
//		}
//	}
//		if (j != 101)
//		{
//			comX = vcpointall[j].row;
//			comY = vcpointall[j].col;
//			return true;
//		}
//		else return false;
//	}
//
//
//
//bool Game::Max_Vc(int left1, int right1, int top1, int bottom1, int row, int col, int depth, int oneself, int oppsite, int max, int last)
//{
//	/*VCPoint*vcpoint;
//	vcpoint = new VCPoint[100]();*/
//	VCPoint vcpointoneself[100];
//	VCPoint vcpointall[100];
//	VCPoint vcpointoppsite[100];
//	int temp = 999;
//	for (int i = 0; i < 100; i++)
//	{
//		vcpointoneself[i].row = 0;
//		vcpointoneself[i].col = 0;
//		vcpointoneself[i].pri = 0;
//
//		vcpointall[i].row = 0;
//		vcpointall[i].col = 0;
//		vcpointall[i].pri = 0;
//
//		vcpointoppsite[i].row = 0;
//		vcpointoppsite[i].col = 0;
//		vcpointoppsite[i].pri = 0;
//	}
//	if (max >= 10000)
//	{
//		if (GetCheng5ChangLian(row, col, oppsite))
//			return false;
//
//		if (Judge_IsHuo4ShuangChong4(row, col, oppsite))//������ģ����Ҷ��˻����
//			return false;
//	}
//	if (GetCheng5ChangLian(row, col, oppsite))
//	{
//		return false;
//	}
//	if (depth <= 0)
//	{
//		return false;
//	}
//	bool val = false;
//	int num = 0;
//	Chang_UpddateLimit(left1, right1, top1, bottom1, row, col);
//	int num1 = FindVcOneselfPoint(left1, right1, top1, bottom1, oneself, vcpointoneself);//������Ϊ�Լ�
//	int num2 = FindVcOneselfPoint(left1, right1, top1, bottom1, oppsite, vcpointoppsite);//����˵��Ϊ������
//	num = FindVc(vcpointoneself, vcpointoppsite, num1, num2, last, vcpointall, oneself, oppsite);
//
//	for (int i = 0; i <num; i++)
//	{
//
//		for (int k = 0; k < num1; k++)
//		{
//			if ((vcpointall[i].row == vcpointoneself[k].row) && (vcpointoneself[k].col == vcpointall[i].col))
//			{
//				temp = k;
//				break;
//			}
//		}
//		if (temp != 999)
//		{
//			chess[vcpointall[i].row][vcpointall[i].col].flag = oneself;
//			val = Min_Vc(left1, right1, top1, bottom1, vcpointall[i].row, vcpointall[i].col, depth - 1, oneself, oppsite, vcpointall[0].pri, vcpointoneself[temp].pri);
//			chess[vcpointall[i].row][vcpointall[i].col].flag = SPACE;
//			if (val)
//			{
//				return true;//������һ����û��һ������Զ�ס�����Խض�
//			}
//		}
//		
//	}
//	return  false;
//}
//
//
////����Max��ֹvc���� ��wo���������ĳ�ͻ
//bool Game::Min_Vc(int left1, int right1, int top1, int bottom1, int row, int col, int depth, int oneself, int oppsite, int max, int last)
//{
//	/*VCPoint*vcpoint;
//	vcpoint = new VCPoint[100]();*/
//	VCPoint vcpointoneself[100];
//	VCPoint vcpointoppsite[100];
//	VCPoint vcpointall[100];
//	int temp = 999;
//	for (int i = 0; i < 100; i++)
//	{
//		vcpointoneself[i].row = 0;
//		vcpointoneself[i].col = 0;
//		vcpointoneself[i].pri = 0;
//
//		vcpointall[i].row = 0;
//		vcpointall[i].col = 0;
//		vcpointall[i].pri = 0;
//
//
//		vcpointoppsite[i].row = 0;
//		vcpointoppsite[i].col = 0;
//		vcpointoppsite[i].pri = 0;
//
//	}
//
//	bool val = true;
//	if (max >= 10000)
//	{
//		if (GetCheng5ChangLian(row, col, oneself))
//			return true;
//		if (Judge_IsHuo4ShuangChong4(row, col, oneself))
//			return true;
//
//	}
//	if (GetCheng5ChangLian(row, col, oneself))
//	{
//		return true;
//	}
//	if (depth <= 0)
//	{
//		return false;
//	}
//
//	Chang_UpddateLimit(left1, right1, top1, bottom1, row, col);
//	int num;
//	int num1= FindVcOneselfPoint(left1, right1, top1, bottom1, oneself, vcpointoneself);//������Ϊ�Լ�
//	int num2= FindVcOneselfPoint(left1, right1, top1, bottom1, oppsite, vcpointoppsite);//����˵��Ϊ������
//	num = FindVc(vcpointoneself, vcpointoppsite, num1, num2, last, vcpointall, oppsite, oneself);
//
//	for (int i = 0; i < num; i++)
//	{
//		for (int k = 0; k < num1; k++)
//		{
//			if ((vcpointall[i].row == vcpointoneself[k].row) && (vcpointoneself[k].col == vcpointall[i].col))
//			{
//				temp = k;
//				break;
//			}
//		}
//		if (temp != 999)
//		{
//			chess[vcpointall[i].row][vcpointall[i].col].flag = oppsite;
//			val = Max_Vc(left1, right1, top1, bottom1, vcpointall[i].row, vcpointall[i].col, depth - 1, oneself, oppsite, vcpointall[0].pri, vcpointoneself[temp].pri);
//			chess[vcpointall[i].row][vcpointall[i].col].flag = SPACE;
//			if (!val)
//			{
//				return false;//����һ�����Զ�ס�ĵ�ͷ���
//			}
//		}
//		
//
//	}
//	return true;//���е㶼���ܶ�ס�ͷ��سɹ�
//}
//
//bool Game::Iteration()
//{
//	for (int i = 4; i <= 24; i += 2)
//	{
//		if (MaxMin_Vc(i))
//		{
//			return true;
//		}
//
//	}
//	return false;
//}
////������maxmin_vcʹ��
//int Game::FindVcPoint(int left1, int right1, int top1, int bottom1, int oneself, int oppsite, VCPoint p[])
//{
//
//	int cnt = 0;
//	for (int i = top1; i <= bottom1; ++i)
//	{
//		for (int j = left1; j <= right1; ++j)
//		{
//			if (chess[i][j].flag != SPACE)
//				continue;
//			if (BLACK_CHESS == oneself && IsKinjite(i, j)) // �Լ��Ľ��ֵ�
//				continue;
//
//			int res = IsCombForAllVC(i, j, oneself, oppsite);
//			//�ǹ������򱣴�,�������ͷ�2�֣�VCF����VCF��VCT���Ϲ���
//
//			if (!res)
//				continue;
//
//			p[cnt++].init(i, j, res);
//			if (cnt >= 100)
//			{
//				i = j = bottom1;
//			}
//		}
//	}
//	sort(p, p + cnt);  //�����ȼ�����
//	return cnt;
//
//}
//
////��������������һ��
//int Game::IsCombForAllVC(int row, int col, int oneself, int oppsite)
//{
//	//�ж��Ƿ��� VC �����Ĺ����㣬���������ȼ�
//	Search(row, col, oneself);
//	int vc_value = 0;
//	if (cheng5)
//		vc_value += 10000;
//	if (huo4)
//		vc_value += 500;
//	if (chong4)
//		vc_value += 150;
//	if (tiaochong4)
//		vc_value += 140;
//	if (lian3)
//		vc_value += 30;
//	if (tiao3)
//		vc_value += 25;
//	Search(row, col, oppsite);
//	if (cheng5)
//		vc_value += 10000;
//	if (huo4)
//		vc_value += 500;
//	return vc_value;
//}//��˫����vc
//int Game::FindVc(VCPoint p[], VCPoint opp[], int num, int numopp, int last, VCPoint all[], int oneself, int oppsite)
//{
//	int cnt = 0;
//	for (int i = 0; i < num; i++)
//	{
//		if (p[i].pri >last)
//		{
//
//			int res = IsCombForAllVC(p[i].row, p[i].col, oneself, oppsite);
//			all[cnt++].init(p[i].row, p[i].col, res);
//		}
//	}
//	for (int j = 0; j < numopp; j++)
//	{
//		if (opp[j].pri > last)
//		{
//
//			int res = IsCombForAllVC(opp[j].row, opp[j].col, oneself, oppsite);
//			all[cnt++].init(opp[j].row, opp[j].col, res);
//		}
//	}
//	sort(p, p + cnt);
//	return cnt;
//}
////��������������һ��
//int Game::IsCombForOneselfVc(int row, int col, int oneself)
//{
//	Search(row, col, oneself);
//	if (cheng5)
//		return 10000;
//	if (huo4)
//		return 500;
//	if (chong4)
//		return 150;
//	if (tiaochong4)
//		return 150;
//	if (lian3)
//		return  30;
//	if (tiao3)
//		return 30;
//	return 0;
//}
//int Game::FindVcOneselfPoint(int left1, int right1, int top1, int bottom1, int oneself, VCPoint p[])//oneself �ĵ�����
//{
//	int cnt = 0;
//	for (int i = top1; i <= bottom1; ++i)
//	{
//		for (int j = left1; j <= right1; ++j)
//		{
//			if (chess[i][j].flag != SPACE)
//				continue;
//			if (BLACK_CHESS == oneself && IsKinjite(i, j)) // �Լ��Ľ��ֵ�
//				continue;
//
//			int res = IsCombForOneselfVc(i, j, oneself);
//			//�ǹ������򱣴�,�������ͷ�2�֣�VCF����VCF��VCT���Ϲ���
//
//			if (!res)
//				continue;
//
//			p[cnt++].init(i, j, res);
//			if (cnt >= 100)
//			{
//				i = j = bottom1;
//			}
//		}
//	}
//	sort(p, p + cnt);  //�����ȼ�����
//	return cnt;
//
//
//}
//
//int Game::IsCombForVC(int row, int col, int oneself, int oppsite)
//{
//	//�ж��Ƿ��� VC �����Ĺ����㣬���������ȼ�
//	Search(row, col, oneself);
//	int vc_value = 0;
//	if (cheng5)
//		vc_value += 10000;
//	if (huo4)
//		vc_value += 500;
//	if (chong4)
//		vc_value += 150;
//	if (tiaochong4)
//		vc_value += 140;
//	if (lian3)
//		vc_value += 30;
//	if (tiao3)
//		vc_value += 25;
//	Search(row, col, oppsite);
//	if (cheng5)
//		vc_value += 10000;
//	if (huo4)
//		vc_value += 500;
//	return vc_value;
//}//��˫����vc
//
//bool Game::MaxMin_Vc(int depth)
//{
//	int j = 101;
//	bool val;
//	/*VCPoint*vcpoint;
//	vcpoint = new VCPoint[100]();
//	*/
//	VCPoint vcpoint[100];
//	for (int i = 0; i < 100; i++)
//	{
//		vcpoint[i].row = 0;
//		vcpoint[i].col = 0;
//		vcpoint[i].pri = 0;
//	}
//	int num = FindVcPoint(left, right, top, bottom, cComputer, cPeople, vcpoint);
//	for (int i = 0; i < num; i++)
//	{
//		chess[vcpoint[i].row][vcpoint[i].col].flag = cComputer;
//		val = Min_Vc(left, right, top, bottom, vcpoint[i].row, vcpoint[i].col, depth - 1, cComputer, cPeople, vcpoint[0].pri);
//		chess[vcpoint[i].row][vcpoint[i].col].flag = SPACE;
//		if (val)
//		{
//			j = i;
//			break;//������һ����û��һ������Զ�ס�����Խض�
//		}
//
//	}
//	if (j != 101)
//	{
//		comX = vcpoint[j].row;
//		comY = vcpoint[j].col;
//		return true;
//	}
//	else return false;
//	//if (j == 101)//û�й����� ,�鿴���ֽ�����
//	//{
//
//	//	int num = FindVcPoint(left, right, top, bottom, cPeople, defend);
//	//	for (int i = 0; i < num; i++)
//	//	{
//	//		chess[defend[i].row][defend[i].col].flag = cPeople;
//	//		val = Min_Vc(left, right, top, bottom, defend[i].row, defend[i].col, depth - 1, cPeople, cComputer);
//	//		chess[defend[i].row][defend[i].col].flag = SPACE;
//	//		if (val)
//	//		{
//	//			j = i;
//	//			break;//������һ����û��һ������Զ�ס�����Խض�
//	//		}
//
//	//	}
//	//	if (j == 101)
//	//	{
//	//		return false;
//	//	}
//	//	else {
//	//		int j = 101;//��λj
//	//		int i1 = 101;
//	//		VCPoint vc_iskin[101];
//	//		if (IsKinjite(defend[j].row, defend[j].col))//��ֹ�����������ֵ㣬��������
//	//		{
//	//			poinT result[11];
//
//	//			int num = heuristic(left, right, top, bottom, cComputer, result);//������  Ϊ����
//	//			for (int i = 0; i < num; i++)//���ѭ������ģ�����������һ����
//	//			{
//	//				if (chess[result[i].x][result[i].y].flag != SPACE)
//	//					continue;
//	//				if (BLACK_CHESS == cComputer && IsKinjite(result[i].x, result[i].y)) //������������Լ��Ľ��ֵ�
//	//					continue;
//	//				chess[result[i].x][result[i].y].flag = cComputer;
//	//				int num = FindVcPoint(left, right, top, bottom, cPeople, vc_iskin);
//	//				for (int m = 0; m < num; m++)
//	//				{
//	//					chess[vc_iskin[m].row][vc_iskin[m].col].flag = cPeople;
//	//					val = Min_Vc(left, right, top, bottom, vc_iskin[m].row, vc_iskin[m].col, depth - 2, cPeople, cComputer);
//	//					chess[vc_iskin[m].row][vc_iskin[m].col].flag = SPACE;
//	//					if (val)
//	//					{
//	//						j = i;
//	//						break;//������һ����û��һ������Զ�ס�����Խض�
//	//					}
//
//	//				}
//	//				if (j != 101)
//	//				{
//	//					i1 = i;
//	//					break;
//	//				}
//	//				chess[result[i].x][result[i].y].flag = SPACE;//����
//
//	//			}
//	//			if (IsKinjite(result[i1].x, result[i1].y))
//	//			{
//	//				return false;
//	//			}
//	//			else {
//	//				   comX = result[i1].x;
//	//				   comY = result[i1].y;
//	//				   return true;
//
//	//			}
//	//		}
//
//	//		
//	//	}
//	//}
//	////�Լ������Ƿ���Ч���Ƿ񱻽ض�
//	//else {
//	//	  int  j1 = 101;
//	//	chess[vcpoint[j].row][vcpoint[j].col].flag = cComputer;
//	//	if (!GetCheng5ChangLian(vcpoint[j].row, vcpoint[j].col, cComputer))
//	//	{
//	//		int num = FindVcPoint(left, right, top, bottom, cPeople, defend);
//	//		for (int i = 0; i < num; i++)
//	//		{
//	//			chess[defend[i].row][defend[i].col].flag = cPeople;
//	//			val = Min_Vc(left, right, top, bottom, defend[i].row, defend[i].col, depth - 1, cPeople, cComputer);
//	//			chess[defend[i].row][defend[i].col].flag = SPACE;
//	//			if (val)
//	//			{
//	//				j1 = i;
//	//				break;//������һ����û��һ������Զ�ס�����Խض�
//	//			}
//
//	//		}
//	//	}
//	//	if (GetCheng5ChangLian(vcpoint[j].row, vcpoint[j].col, cComputer)) //��ֹ�Ѿ�����㣬������,��ʵ���������ã�����֪���ͺ�
//	//	{
//	//		j1 = 101;
//	//	}
//	//	chess[vcpoint[j].row][vcpoint[j].col].flag = SPACE;
//
//	//	if (j1 == 101)//û�з�����
//	//	{
//	//		comX = vcpoint[j].row;
//	//		comY = vcpoint[j].col;
//	//		return true;
//	//	}
//	//	else { //�з����㣬�ҿ���תΪ����
//
//	//		comX = defend[j1].row;
//	//		comY = defend[j1].col;
//	//		return true;
//	//	}
//
//	//}
//
//}
//
//bool Game::Max_Vc(int left1, int right1, int top1, int bottom1, int row, int col, int depth, int oneself, int oppsite, int max)
//{
//	/*VCPoint*vcpoint;
//	vcpoint = new VCPoint[100]();*/
//	VCPoint vcpoint[100];
//	for (int i = 0; i < 100; i++)
//	{
//		vcpoint[i].row = 0;
//		vcpoint[i].col = 0;
//		vcpoint[i].pri = 0;
//	}
//	if (max >= 10000)
//	{
//		if (GetCheng5ChangLian(row, col, oppsite))
//			return false;
//
//		if (Judge_IsHuo4ShuangChong4(row, col, oppsite))//������ģ����Ҷ��˻����
//			return false;
//	}
//	if (GetCheng5ChangLian(row, col, oppsite))
//	{
//		return false;
//	}
//	if (depth <= 0)
//	{
//		return false;
//	}
//	bool val = false;
//	int num = 0;
//	Chang_UpddateLimit(left1, right1, top1, bottom1, row, col);
//	num = FindVcPoint(left1, right1, top1, bottom1, oneself, oppsite, vcpoint);//������Ϊ�Լ�
//	for (int i = 0; i <num; i++)
//	{
//		chess[vcpoint[i].row][vcpoint[i].col].flag = oneself;
//		val = Min_Vc(left1, right1, top1, bottom1, vcpoint[i].row, vcpoint[i].col, depth - 1, oneself, oppsite, vcpoint[0].pri);
//		chess[vcpoint[i].row][vcpoint[i].col].flag = SPACE;
//		if (val)
//		{
//			return true;//������һ����û��һ������Զ�ס�����Խض�
//		}
//	}
//	return  false;
//}
//
//
////����Max��ֹvc���� ��wo���������ĳ�ͻ
//bool Game::Min_Vc(int left1, int right1, int top1, int bottom1, int row, int col, int depth, int oneself, int oppsite, int max)
//{
//	/*VCPoint*vcpoint;
//	vcpoint = new VCPoint[100]();*/
//	VCPoint vcpoint[100];
//	for (int i = 0; i < 100; i++)
//	{
//		vcpoint[i].row = 0;
//		vcpoint[i].col = 0;
//		vcpoint[i].pri = 0;
//	}
//	bool val = true;
//	if (max >= 10000)
//	{
//		if (GetCheng5ChangLian(row, col, oneself))
//			return true;
//		if (Judge_IsHuo4ShuangChong4(row, col, oneself))
//			return true;
//
//	}
//	if (GetCheng5ChangLian(row, col, oneself))
//	{
//		return true;
//	}
//	if (depth <= 0)
//	{
//		return false;
//	}
//
//	Chang_UpddateLimit(left1, right1, top1, bottom1, row, col);
//	int num = FindVcPoint(left1, right1, top1, bottom1, oppsite, oneself, vcpoint);//������  Ϊ����
//	for (int i = 0; i < num; i++)
//	{
//		chess[vcpoint[i].row][vcpoint[i].col].flag = oppsite;
//		val = Max_Vc(left1, right1, top1, bottom1, vcpoint[i].row, vcpoint[i].col, depth - 1, oneself, oppsite, vcpoint[0].pri);
//		chess[vcpoint[i].row][vcpoint[i].col].flag = SPACE;
//		if (!val)
//		{
//			return false;//����һ�����Զ�ס�ĵ�ͷ���
//		}
//
//	}
//	return true;//���е㶼���ܶ�ס�ͷ��سɹ�
//}
//
//bool Game::Iteration()
//{
//	for (int i = 4; i <= 10; i += 2)
//	{
//		if (MaxMin_Vc(i))
//		{
//			return true;
//		}
//
//	}
//	return false;
//}
//
//int Game::FindVcPoint(int left1, int right1, int top1, int bottom1, int oneself, int oppsite, VCPoint p[])
//{
//
//	int cnt = 0;
//	for (int i = left1; i <= right1; ++i)
//	{
//		for (int j = top1; j <= bottom1; ++j)
//		{
//			if (chess[i][j].flag != SPACE)
//				continue;
//			if (BLACK_CHESS == oneself && IsKinjite(i, j)) // �Լ��Ľ��ֵ�
//				continue;
//
//			int res = IsCombForVC(i, j, oneself, oppsite);
//			//�ǹ������򱣴�,�������ͷ�2�֣�VCF����VCF��VCT���Ϲ���
//
//			if (!res)
//				continue;
//
//			p[cnt++].init(i, j, res);
//			if (cnt >= 100)
//			{
//				i = j = bottom1;
//			}
//		}
//	}
//	sort(p, p + cnt);  //�����ȼ�����
//	return cnt;
//
//}