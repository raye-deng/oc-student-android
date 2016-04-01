package s.android.ffmpeg.gui.fragment;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

import com.alibaba.fastjson.JSON;

import java.util.List;

import s.android.ffmpeg.R;
import s.android.ffmpeg.adapter.CurriculumAdapter;
import s.android.ffmpeg.util.ConstantUtils;
import s.android.ffmpeg.handler.LoadDataHandler;
import s.android.ffmpeg.handler.ResponseHandler;
import s.android.ffmpeg.http.RequestParams;
import s.android.ffmpeg.model.LessonDto;
import s.android.ffmpeg.util.MediaUtils;
import s.android.ffmpeg.util.RequestUtils;

/**
 * 作者： CodingMates
 * 创建时间： 2015/2/6 11:17.
 */
public class HistoryFragment extends Fragment {
    public static final String CLASS_TAG = "上课记录界面";
    private ListView curriculumsList;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_curriculum, container, false);
        curriculumsList = (ListView) view.findViewById(R.id.curriculumLV);
        return view;
    }

    @Override
    public void onResume() {
        RequestData();
        super.onResume();
    }

    public void RequestData() {
        RequestParams rp = new RequestParams();
        rp.put("memberId", MediaUtils.LOGIN_USER.getMemberId() + "");
        RequestUtils.post(
                ConstantUtils.getURL(ConstantUtils.SERVER_WEB_IP, ConstantUtils.SERVER_WEB_PORT, ConstantUtils.URL_COURSE_HISTORY),
                rp,
                new ResponseHandler(this.getActivity()
                        .getApplicationContext(), new LoadDataHandler() {
                    @Override
                    public void onSuccess(String data) {
                        Log.d(CLASS_TAG, "获取课表数据成功");
                        List<LessonDto> lessons = JSON.parseArray(data, LessonDto.class);
                        if (lessons.size() > 0) {
                            CurriculumAdapter curriculumAdapter = new CurriculumAdapter(getActivity().getApplicationContext(), lessons);
                            curriculumsList.setAdapter(curriculumAdapter);
                            curriculumAdapter.notifyDataSetChanged();
                        } else {
                            curriculumsList.setBackgroundResource(R.drawable.tip_history_no_data);
                        }
                    }

                    @Override
                    public void onStart() {
                    }

                    @Override
                    public void onLoadCaches(String data) {
                    }

                    @Override
                    public void onFinish() {
                    }

                    @Override
                    public void onFailure(String error, String message) {
                        curriculumsList.setBackgroundResource(R.drawable.tip_net_work_failed);
                    }
                })
        );
    }
}
